#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>

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

int Config::get_max_retries() const {
    std::string value = get("max_retries", "3");
    try {
        return std::stoi(value);
    } catch (...) {
        return 3; // Default to 3 if parsing fails
    }
}

int Config::get_connect_timeout() const {
    std::string value = get("connect_timeout", "10");
    try {
        return std::stoi(value);
    } catch (...) {
        return 10; // Default to 10 seconds
    }
}

int Config::get_read_timeout() const {
    std::string value = get("read_timeout", "30");
    try {
        return std::stoi(value);
    } catch (...) {
        return 30; // Default to 30 seconds
    }
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

FileCache::FileCache(const Config& config, const std::string& cache_dir, const std::string& upstream_host)
    : m_config(config), m_cache_dir(cache_dir), m_upstream_host(upstream_host) {
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
    const int MAX_RETRIES = m_config.get_max_retries();
    const int CONNECT_TIMEOUT_SECONDS = m_config.get_connect_timeout();
    const int READ_TIMEOUT_SECONDS = m_config.get_read_timeout();

    for (int retry = 0; retry < MAX_RETRIES; retry++) {
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

            // Set connect timeout
            stream.expires_after(std::chrono::seconds(CONNECT_TIMEOUT_SECONDS));

            // Connect to host
            stream.connect(results);

            // Set read timeout (reset for read operation)
            stream.expires_after(std::chrono::seconds(READ_TIMEOUT_SECONDS));

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
                std::cerr << "Upstream returned HTTP " << res.result_int()
                          << " for " << request_path << std::endl;
                // Don't retry on client errors (4xx), but retry on server errors (5xx)
                if (res.result_int() >= 400 && res.result_int() < 500) {
                    return false;
                }
                throw std::runtime_error("HTTP error: " + std::to_string(res.result_int()));
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

            std::cout << "Successfully fetched: " << request_path << std::endl;
            return true;

        } catch (const beast::system_error& e) {
            if (retry == MAX_RETRIES - 1) {
                std::cerr << "Failed to fetch " << request_path
                          << " after " << MAX_RETRIES << " attempts: " << e.what() << std::endl;
                return false;
            }

            // Exponential backoff: 1s, 2s, 4s
            int backoff_seconds = 1 << retry;
            std::cerr << "Fetch failed (attempt " << (retry + 1) << "/" << MAX_RETRIES
                      << "): " << e.what() << ", retrying in " << backoff_seconds << "s..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(backoff_seconds));

        } catch (const std::exception& e) {
            std::cerr << "Error fetching from upstream: " << e.what() << std::endl;
            return false;
        }
    }

    return false;
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
            return std::shared_ptr<http::response<http::file_body>>(nullptr);
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
            return std::shared_ptr<http::response<http::file_body>>(nullptr);
    }

    // Set response headers
    response->set(http::field::content_type, "application/octet-stream");
    response->set(http::field::server, "pacPrism/0.1.0");
    response->set(http::field::last_modified, get_last_modified(cache_path));
    response->set(http::field::etag, generate_etag(cache_path));
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
            return std::shared_ptr<http::response<http::file_body>>(nullptr);
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
            return std::shared_ptr<http::response<http::file_body>>(nullptr);
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

//==============================================================================
// Conditional Request Support
//==============================================================================

std::string FileCache::get_last_modified(const std::string& cache_path) const {
    try {
        auto ftime = fs::last_write_time(cache_path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

        // Format as HTTP date (RFC 1123)
        char buffer[80];
        std::tm tm;
        localtime_r(&cftime, &tm);
        std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm);
        return std::string(buffer);
    } catch (const std::exception& e) {
        std::cerr << "Failed to get last modified time: " << e.what() << std::endl;
        return "";
    }
}

std::string FileCache::generate_etag(const std::string& cache_path) const {
    try {
        // Simple ETag: "size-modtime"
        auto ftime = fs::last_write_time(cache_path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t mod_time = std::chrono::system_clock::to_time_t(sctp);
        std::size_t file_size = fs::file_size(cache_path);

        return std::format("\"{}-{}\"", file_size, mod_time);
    } catch (const std::exception& e) {
        std::cerr << "Failed to generate ETag: " << e.what() << std::endl;
        return "";
    }
}

std::time_t FileCache::parse_http_date(const std::string& date_str) const {
    // Parse RFC 1123 date format: "Wed, 21 Oct 2015 07:28:00 GMT"
    std::tm tm = {};
    std::istringstream ss(date_str);

    // Try to parse HTTP date format
    ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
    if (ss.fail()) {
        return 0;  // Parse failed
    }

    return std::mktime(&tm);
}

bool FileCache::check_modified_since(const std::string& if_modified_since, const std::string& cache_path) const {
    if (if_modified_since.empty()) {
        return true;  // No header, assume modified
    }

    try {
        auto ftime = fs::last_write_time(cache_path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t file_mod_time = std::chrono::system_clock::to_time_t(sctp);

        std::time_t if_time = parse_http_date(if_modified_since);
        if (if_time == 0) {
            return true;  // Parse failed, assume modified
        }

        // File is modified if file_mod_time > if_time
        return file_mod_time > if_time;
    } catch (const std::exception& e) {
        std::cerr << "Failed to check modified since: " << e.what() << std::endl;
        return true;  // On error, assume modified
    }
}

file_cache_response FileCache::get_or_fetch_with_conditional(
    const std::string& request_path,
    unsigned http_version,
    const std::string& if_modified_since,
    const std::string& if_none_match
) {
    // Ensure file is cached first
    if (!is_cached(request_path)) {
        std::cout << "Cache miss for: " << request_path << ", fetching from upstream..." << std::endl;
        if (!fetch_from_upstream(request_path)) {
            std::cerr << "Failed to fetch: " << request_path << std::endl;
            return std::shared_ptr<http::response<http::file_body>>(nullptr);
        }
        std::cout << "Successfully cached: " << request_path << std::endl;
    }

    std::string cache_path = get_cache_path(request_path);

    // Check If-Modified-Since
    if (!if_modified_since.empty()) {
        bool is_modified = check_modified_since(if_modified_since, cache_path);

        if (!is_modified) {
            // File not modified, return HTTP 304
            std::cout << "Conditional request: Not modified (304) for " << request_path << std::endl;

            auto response = std::make_shared<http::response<http::empty_body>>(http::status::not_modified, http_version);
            response->set(http::field::server, "pacPrism/0.1.0");
            response->set(http::field::date, get_last_modified(cache_path));
            response->set(http::field::etag, generate_etag(cache_path));
            response->prepare_payload();

            return response;
        }
    }

    // Check If-None-Match (ETag)
    if (!if_none_match.empty()) {
        std::string current_etag = generate_etag(cache_path);
        if (current_etag == if_none_match) {
            // ETag matches, return HTTP 304
            std::cout << "Conditional request: ETag match (304) for " << request_path << std::endl;

            auto response = std::make_shared<http::response<http::empty_body>>(http::status::not_modified, http_version);
            response->set(http::field::server, "pacPrism/0.1.0");
            response->set(http::field::date, get_last_modified(cache_path));
            response->set(http::field::etag, current_etag);
            response->prepare_payload();

            return response;
        }
    }

    // File has been modified or no conditional headers
    // Return normal response with ETag and Last-Modified headers
    auto response = std::make_shared<http::response<http::file_body>>(http::status::ok, http_version);

    beast::error_code ec;
    response->body().open(cache_path.c_str(), beast::file_mode::read, ec);

    if (ec) {
            std::cerr << "Failed to open cached file: " << cache_path << " - " << ec.message() << std::endl;
            return std::shared_ptr<http::response<http::file_body>>(nullptr);
    }

    response->set(http::field::content_type, "application/octet-stream");
    response->set(http::field::server, "pacPrism/0.1.0");
    response->set(http::field::last_modified, get_last_modified(cache_path));
    response->set(http::field::etag, generate_etag(cache_path));
    response->content_length(response->body().size());

    response->prepare_payload();

    std::cout << "Conditional request: Modified (200) for " << request_path << std::endl;

    return response;
}