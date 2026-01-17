#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <memory>

#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace fs = std::filesystem;

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
    unsigned short get_port() const;
    std::string get_bind_address() const;

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
    FileCache(const std::string& cache_dir, const std::string& upstream_host);
    ~FileCache() = default;

    // Get file from cache or fetch from upstream
    // Returns a file response if successful, nullptr on failure
    std::shared_ptr<http::response<http::file_body>> get_or_fetch(
        const std::string& request_path,
        unsigned http_version
    );

    // Check if file exists in cache
    bool is_cached(const std::string& request_path) const;

    // Get the local file path for a given request path
    std::string get_cache_path(const std::string& request_path) const;

    // Set cache directory
    void set_cache_dir(const std::string& cache_dir);

private:
    // Fetch file from upstream and cache it
    bool fetch_from_upstream(const std::string& request_path);

    // Ensure cache directory exists
    void ensure_cache_dir();

    // Build upstream URL
    std::string build_upstream_url(const std::string& request_path) const;

private:
    fs::path m_cache_dir;
    std::string m_upstream_host;
};
