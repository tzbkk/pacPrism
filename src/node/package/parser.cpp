#include <node/package/parser.hpp>

std::optional<package_info> PackageParser::parse(const std::string& path) {
    package_info info;

    // Check if path starts with /debian/pool/
    if (path.rfind("/debian/pool/", 0) != 0) {
        return std::nullopt;
    }

    // Extract component (main/contrib/non-free)
    // "/debian/pool/" is 13 characters (positions 0-12), so component starts at position 13
    size_t component_start = 13;
    size_t component_end = path.find('/', component_start);
    if (component_end == std::string::npos) {
        return std::nullopt;
    }
    info.component = path.substr(component_start, component_end - component_start);
    if (info.component != "main" && info.component != "contrib" && info.component != "non-free") {
        return std::nullopt;
    }

    // Extract filename from path (last segment)
    size_t last_slash = path.find_last_of('/');
    if (last_slash == std::string::npos) {
        return std::nullopt;
    }
    std::string filename = path.substr(last_slash + 1);

    // Find first underscore (separates package name and version)
    size_t first_underscore = filename.find('_');
    if (first_underscore == std::string::npos) {
        return std::nullopt;
    }
    info.name = filename.substr(0, first_underscore);
    if (info.name.empty()) {
        return std::nullopt;
    }

    // Find last dot (separates extension)
    size_t last_dot = filename.find_last_of('.');
    if (last_dot == std::string::npos || last_dot <= first_underscore) {
        return std::nullopt;
    }

    // Check for source packages first
    // 1. .orig.tar.gz/xz format
    size_t orig_pos = filename.find(".orig");
    if (orig_pos != std::string::npos && orig_pos > first_underscore) {
        info.version = filename.substr(first_underscore + 1, orig_pos - first_underscore - 1);
        info.architecture = "source";
        info.extension = filename.substr(orig_pos);
        return info;
    }

    // 2. .dsc files
    if (filename.compare(last_dot, 4, ".dsc") == 0) {
        info.version = filename.substr(first_underscore + 1, last_dot - first_underscore - 1);
        info.architecture = "source";
        info.extension = ".dsc";
        return info;
    }

    // 3. .tar.gz and .tar.xz files (without .orig)
    if (filename.compare(last_dot, 3, ".gz") == 0 || filename.compare(last_dot, 3, ".xz") == 0) {
        size_t tar_pos = filename.rfind(".tar", last_dot);
        if (tar_pos != std::string::npos && tar_pos > first_underscore) {
            info.version = filename.substr(first_underscore + 1, tar_pos - first_underscore - 1);
            info.architecture = "source";
            info.extension = filename.substr(tar_pos);
            return info;
        }
    }

    // Binary packages: name_version_arch.extension
    size_t second_underscore = filename.find('_', first_underscore + 1);
    if (second_underscore == std::string::npos || second_underscore >= last_dot) {
        return std::nullopt;
    }

    info.version = filename.substr(first_underscore + 1, second_underscore - first_underscore - 1);
    info.architecture = filename.substr(second_underscore + 1, last_dot - second_underscore - 1);
    info.extension = filename.substr(last_dot);

    return info;
}

std::string PackageParser::extract_component(const std::string& path) {
    // This function is deprecated - component is extracted directly in parse()
    return "";
}

std::string PackageParser::extract_package_name(const std::string& path) {
    // This function is deprecated - package name is extracted directly in parse()
    return "";
}

bool PackageParser::extract_version_arch(const std::string& filename, std::string& version, std::string& arch) {
    // This function is deprecated - version/arch are extracted directly in parse()
    return false;
}

std::string PackageParser::extract_extension(const std::string& filename) {
    // This function is deprecated - extension is extracted directly in parse()
    return "";
}
