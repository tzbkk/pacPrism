#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <console/io/io.hpp>

// Boost.Beast HTTP client includes
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

bool Config::load_from_file(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << config_path << std::endl;
        return false;
    }

    std::string line;
    int line_num = 0;
    while (std::getline(file, line)) {
        line_num++;
        if (!parse_line(line)) {
            // Skip invalid lines (comments, empty lines)
            continue;
        }
    }

    file.close();
    return true;
}

std::string Config::get(const std::string& key) const {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        return it->second;
    }
    return "";
}

std::string Config::get(const std::string& key, const std::string& default_value) const {
    std::string value = get(key);
    return value.empty() ? default_value : value;
}

void Config::set(const std::string& key, const std::string& value) {
    m_config[key] = value;
}

bool Config::has(const std::string& key) const {
    return m_config.find(key) != m_config.end();
}

std::string Config::get_upstream() const {
    return get("upstream", "ftp.debian.org");
}

std::string Config::get_cache_dir() const {
    return get("cache_dir", "./cache");
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

bool Config::parse_line(const std::string& line) {
    std::string trimmed = trim(line);

    // Skip empty lines
    if (trimmed.empty()) {
        return false;
    }

    // Skip comments
    if (trimmed[0] == '#') {
        return false;
    }

    // Parse key=value
    size_t pos = trimmed.find('=');
    if (pos == std::string::npos) {
        std::cerr << "Warning: Invalid config line (missing '='): " << trimmed << std::endl;
        return false;
    }

    std::string key = trim(trimmed.substr(0, pos));
    std::string value = trim(trimmed.substr(pos + 1));

    if (key.empty()) {
        std::cerr << "Warning: Invalid config line (empty key): " << trimmed << std::endl;
        return false;
    }

    m_config[key] = value;
    return true;
}

// FileCache Implementation

FileCache::FileCache(const std::string& cache_dir, const std::string& upstream_host)
    : m_cache_dir(cache_dir), m_upstream_host(upstream_host) {
    ensure_cache_dir();
}

void FileCache::ensure_cache_dir() {
    if (!fs::exists(m_cache_dir)) {
        std::error_code ec;
        if (!fs::create_directories(m_cache_dir, ec)) {
            std::cerr << "Warning: Failed to create cache directory: "
                      << m_cache_dir.string() << " - " << ec.message() << std::endl;
        }
    }
}

void FileCache::set_cache_dir(const std::string& cache_dir) {
    m_cache_dir = cache_dir;
    ensure_cache_dir();
}

std::string FileCache::get_cache_path(const std::string& request_path) const {
    // Remove leading slash and convert to filesystem path
    std::string clean_path = (request_path[0] == '/') ? request_path.substr(1) : request_path;

    // Replace URL-encoded characters (basic implementation)
    // For now, just handle the most common case: no encoding
    return (m_cache_dir / clean_path).string();
}

bool FileCache::is_cached(const std::string& request_path) const {
    std::string cache_path = get_cache_path(request_path);
    return fs::exists(cache_path) && fs::is_regular_file(cache_path);
}

std::string FileCache::build_upstream_url(const std::string& request_path) const {
    // Remove leading slash if present
    std::string clean_path = (request_path[0] == '/') ? request_path.substr(1) : request_path;
    return "http://" + m_upstream_host + "/" + clean_path;
}

bool FileCache::fetch_from_upstream(const std::string& request_path) {
    try {
        net::io_context io_ctx;

        // Parse upstream host and port
        std::string host = m_upstream_host;
        std::string port = "80";
        size_t colon_pos = host.find(':');
        if (colon_pos != std::string::npos) {
            port = host.substr(colon_pos + 1);
            host = host.substr(0, colon_pos);
        }

        // Resolve host
        tcp::resolver resolver(io_ctx);
        auto const results = resolver.resolve(host, port);

        // Create TCP stream
        beast::tcp_stream stream(io_ctx);
        stream.connect(results);

        // Build HTTP request
        std::string target = (request_path[0] == '/') ? request_path : "/" + request_path;
        http::request<http::string_body> req{
            http::verb::get, target, 11
        };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, "pacPrism/0.1.0");

        // Send request
        http::write(stream, req);

        // Receive response
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        // Check if response is OK
        if (res.result() != http::status::ok) {
            std::cerr << "Upstream returned error: " << res.result_int() << std::endl;
            return false;
        }

        // Get cache file path
        std::string cache_path = get_cache_path(request_path);

        // Create parent directories
        fs::path file_path(cache_path);
        fs::create_directories(file_path.parent_path());

        // Write response body to file
        std::ofstream outfile(cache_path, std::ios::binary);
        if (!outfile) {
            std::cerr << "Failed to create cache file: " << cache_path << std::endl;
            return false;
        }

        auto& body = res.body();
        auto body_data = body.data();

        // Write the buffer content to file
        for (auto const& buffer : body_data) {
            outfile.write(static_cast<const char*>(buffer.data()), buffer.size());
        }

        outfile.close();

        // Gracefully close connection
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error fetching from upstream: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<http::response<http::file_body>> FileCache::get_or_fetch(
    const std::string& request_path,
    unsigned http_version
) {
    // Check if file is cached
    if (!is_cached(request_path)) {
        std::cout << "Cache miss for: " << request_path << ", fetching from upstream..." << std::endl;
        if (!fetch_from_upstream(request_path)) {
            std::cerr << "Failed to fetch: " << request_path << std::endl;
            return nullptr;
        }
    }

    // Open file for response
    std::string cache_path = get_cache_path(request_path);
    beast::error_code ec;

    // Create file response
    auto response = std::make_shared<http::response<http::file_body>>(http::status::ok, http_version);

    // Open file
    response->body().open(cache_path.c_str(), beast::file_mode::read, ec);

    if (ec) {
        std::cerr << "Failed to open cached file: " << cache_path << " - " << ec.message() << std::endl;
        return nullptr;
    }

    // Set response headers
    response->set(http::field::content_type, "application/octet-stream");
    response->set(http::field::server, "pacPrism/0.1.0");
    response->content_length(response->body().size());

    response->prepare_payload();

    return response;
}

//==============================================================================
// Range Request Support
//==============================================================================

FileCache::RangeInfo FileCache::parse_range_header(const std::string& range_header, const std::string& cache_path) const {
    RangeInfo info;

    // Get file size
    try {
        info.file_size = fs::file_size(cache_path);
    } catch (const std::exception& e) {
        std::cerr << "Failed to get file size: " << e.what() << std::endl;
        return info;
    }

    // Parse Range header: "bytes=start-end"
    // Example: "bytes=0-1023", "bytes=512-", "bytes=-256"
    if (range_header.empty()) {
        return info;
    }

    // Check if it's a bytes range
    const std::string bytes_prefix = "bytes=";
    if (range_header.substr(0, bytes_prefix.length()) != bytes_prefix) {
        return info;
    }

    std::string range_spec = range_header.substr(bytes_prefix.length());

    // Parse start-end
    size_t dash_pos = range_spec.find('-');
    if (dash_pos == std::string::npos) {
        return info;
    }

    std::string start_str = range_spec.substr(0, dash_pos);
    std::string end_str = range_spec.substr(dash_pos + 1);

    try {
        if (start_str.empty()) {
            // Suffix range: "-256" means last 256 bytes
            std::size_t suffix = std::stoull(end_str);
            if (suffix > info.file_size) {
                info.start = 0;
            } else {
                info.start = info.file_size - suffix;
            }
            info.end = info.file_size - 1;
        } else if (end_str.empty()) {
            // Open-ended range: "512-" means from 512 to end
            info.start = std::stoull(start_str);
            info.end = info.file_size - 1;
        } else {
            // Normal range: "0-1023"
            info.start = std::stoull(start_str);
            info.end = std::stoull(end_str);
        }

        // Validate range
        if (info.start >= info.file_size || info.end >= info.file_size || info.start > info.end) {
            std::cerr << "Invalid range: start=" << info.start << ", end=" << info.end
                      << ", file_size=" << info.file_size << std::endl;
            return info;
        }

        info.valid = true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse range: " << e.what() << std::endl;
        return info;
    }

    return info;
}

std::shared_ptr<http::response<http::file_body>> FileCache::get_or_fetch_with_range(
    const std::string& request_path,
    unsigned http_version,
    const std::string& range_header
) {
    // Ensure file is cached first
    if (!is_cached(request_path)) {
        std::cout << "Cache miss for: " << request_path << ", fetching from upstream..." << std::endl;
        if (!fetch_from_upstream(request_path)) {
            std::cerr << "Failed to fetch: " << request_path << std::endl;
            return nullptr;
        }
        std::cout << "Successfully cached: " << request_path << std::endl;
    }

    std::string cache_path = get_cache_path(request_path);

    // Parse Range header
    RangeInfo range = parse_range_header(range_header, cache_path);

    // If no valid Range header, return normal response
    if (!range.valid || range_header.empty()) {
        return get_or_fetch(request_path, http_version);
    }

    // Create HTTP 206 Partial Content response
    auto response = std::make_shared<http::response<http::file_body>>(http::status::partial_content, http_version);

    beast::error_code ec;

    // Open file with seek to start position
    response->body().open(cache_path.c_str(), beast::file_mode::read, ec);
    if (ec) {
        std::cerr << "Failed to open cached file: " << cache_path << " - " << ec.message() << std::endl;
        return nullptr;
    }

    // Set file offset to range start
    response->body().seek(range.start, ec);
    if (ec) {
        std::cerr << "Failed to seek in file: " << ec.message() << std::endl;
        return nullptr;
    }

    // Set Content-Length to the range size
    std::size_t content_length = range.end - range.start + 1;
    response->content_length(content_length);

    // Set response headers
    response->set(http::field::content_type, "application/octet-stream");
    response->set(http::field::server, "pacPrism/0.1.0");
    response->set(http::field::accept_ranges, "bytes");

    // Set Content-Range header: "bytes start-end/total"
    std::string content_range = std::format("bytes {}-{}/{}",
        range.start, range.end, range.file_size);
    response->set(http::field::content_range, content_range);

    response->prepare_payload();

    std::cout << "Range request: " << request_path
              << " (" << range.start << "-" << range.end << "/" << range.file_size << ")" << std::endl;

    return response;
}