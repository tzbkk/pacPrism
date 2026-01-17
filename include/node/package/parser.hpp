// Debian package path parser
// Extracts package information from Debian repository paths
#pragma once

#include <string>
#include <optional>

// Package information extracted from path
struct package_info {
    std::string name;           // Package name (e.g., "vim", "openssh-server")
    std::string version;        // Package version (e.g., "9.0.0", "9.0")
    std::string component;      // Repository component (main/contrib/non-free)
    std::string extension;      // File extension (.deb, .dsc, .tar.xz, etc.)
    std::string architecture;   // Architecture (amd64, i386, all, etc.) - optional
};

// Parser for Debian package repository paths
class PackageParser {
public:
    // Parse a Debian repository path and extract package information
    // Returns std::optional<package_info> - empty if parsing fails
    //
    // Supported path formats:
    // - /debian/pool/main/v/vim/vim_9.0.0.deb
    // - /debian/pool/contrib/o/openssh/openssh-server_9.0.deb
    // - /debian/pool/non-free/libp/libpng/libpng_1.6.0.orig.tar.xz
    //
    // Path structure: /debian/pool/{component}/{first_char}/{package_name}/{package_name}_{version}_{arch}.{ext}
    static std::optional<package_info> parse(const std::string& path);

private:
    // Extract component from path (main/contrib/non-free)
    static std::string extract_component(const std::string& path);

    // Extract package name from path
    static std::string extract_package_name(const std::string& path);

    // Extract version and architecture from filename
    static bool extract_version_arch(const std::string& filename, std::string& version, std::string& arch);

    // Extract file extension
    static std::string extract_extension(const std::string& filename);
};
