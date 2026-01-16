#pragma once

#include <string>
#include <unordered_map>

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
