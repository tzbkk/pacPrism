#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <console/io/io.hpp>

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
    return get("upstream", "debian.org");
}

unsigned short Config::get_port() const {
    std::string port_str = get("port", "9001");
    try {
        int port = std::stoi(port_str);
        if (port < 1 || port > 65535) {
            return 9001;
        }
        return static_cast<unsigned short>(port);
    } catch (...) {
        return 9001;
    }
}

std::string Config::get_bind_address() const {
    return get("bind_address", "0.0.0.0");
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