#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <memory>

#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

namespace fs = std::filesystem;

// Variant type for FileCache responses that can be either file_body or empty_body
using file_cache_response = std::variant<
    std::shared_ptr<http::response<http::file_body>>,
    std::shared_ptr<http::response<http::empty_body>>
>;

// Configuration reader for pacPrism
class Config {
public:
    Config() = default;

    // Load configuration from file
    // Returns true if successful, false otherwise
    bool load_from_file(const std::string& config_path);

    // Get configuration value by key
    // Returns empty string if key not found
    std::string get(const std::string& key) const;

    // Get configuration value with default
    std::string get(const std::string& key, const std::string& default_value) const;

    // Set configuration value
    void set(const std::string& key, const std::string& value);

    // Check if key exists
    bool has(const std::string& key) const;

    // Get specific configuration values
    std::string get_upstream() const;
    std::string get_cache_dir() const;

    // Get error handling configuration
    int get_max_retries() const;
    int get_connect_timeout() const;
    int get_read_timeout() const;

private:
    std::unordered_map<std::string, std::string> m_config;

    // Helper: trim whitespace from string
    static std::string trim(const std::string& str);

    // Helper: parse key=value line
    bool parse_line(const std::string& line);
};

// File cache manager for pacPrism
// Handles caching of package files from upstream mirrors
class FileCache {
public:
    FileCache(const Config& config, const std::string& cache_dir, const std::string& upstream_host);
    ~FileCache() = default;

    // Get file from cache or fetch from upstream
    // Returns a file response if successful, nullptr on failure
    std::shared_ptr<http::response<http::file_body>> get_or_fetch(
        const std::string& request_path,
        unsigned http_version
    );

    // Get file with Range support
    // Returns HTTP 206 for Range requests, HTTP 200 for normal requests
    std::shared_ptr<http::response<http::file_body>> get_or_fetch_with_range(
        const std::string& request_path,
        unsigned http_version,
        const std::string& range_header
    );

    // Check if file exists in cache
    bool is_cached(const std::string& request_path) const;

    // Get the local file path for a given request path
    std::string get_cache_path(const std::string& request_path) const;

    // Set cache directory
    void set_cache_dir(const std::string& cache_dir);

    // Get file with conditional request support (If-Modified-Since, If-None-Match)
    // Returns HTTP 304 (empty_body) if not modified, HTTP 200/206 (file_body) if modified
    file_cache_response get_or_fetch_with_conditional(
        const std::string& request_path,
        unsigned http_version,
        const std::string& if_modified_since,
        const std::string& if_none_match
    );

private:
    // Fetch file from upstream and cache it
    bool fetch_from_upstream(const std::string& request_path);

    // Ensure cache directory exists
    void ensure_cache_dir();

    // Build upstream URL
    std::string build_upstream_url(const std::string& request_path) const;

private:
    const Config& m_config;
    fs::path m_cache_dir;
    std::string m_upstream_host;

    // Helper: Parse Range header (e.g., "bytes=0-1023")
    struct RangeInfo {
        bool valid = false;
        std::size_t start = 0;
        std::size_t end = 0;
        std::size_t file_size = 0;
    };
    RangeInfo parse_range_header(const std::string& range_header, const std::string& cache_path) const;

    // Helper: Get file modification time as HTTP date format
    std::string get_last_modified(const std::string& cache_path) const;

    // Helper: Generate ETag from file size and modification time
    std::string generate_etag(const std::string& cache_path) const;

    // Helper: Check if file has been modified based on If-Modified-Since header
    bool check_modified_since(const std::string& if_modified_since, const std::string& cache_path) const;

    // Helper: Parse HTTP date format
    std::time_t parse_http_date(const std::string& date_str) const;
};
