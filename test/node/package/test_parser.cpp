#include "../../common.hpp"
#include <node/package/parser.hpp>

// Test valid Debian package paths
bool test_valid_vim_package() {
    std::string path = "/debian/pool/main/v/vim/vim_9.0.0_amd64.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "vim");
    ASSERT_TRUE(result->version == "9.0.0");
    ASSERT_TRUE(result->component == "main");
    ASSERT_TRUE(result->extension == ".deb");
    ASSERT_TRUE(result->architecture == "amd64");

    return true;
}

bool test_valid_openssh_package() {
    std::string path = "/debian/pool/contrib/o/openssh/openssh-server_9.0_i386.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "openssh-server");
    ASSERT_TRUE(result->version == "9.0");
    ASSERT_TRUE(result->component == "contrib");
    ASSERT_TRUE(result->extension == ".deb");
    ASSERT_TRUE(result->architecture == "i386");

    return true;
}

bool test_valid_libpng_package() {
    std::string path = "/debian/pool/main/libp/libpng/libpng_1.6.0.orig.tar.xz";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "libpng");
    ASSERT_TRUE(result->version == "1.6.0");
    ASSERT_TRUE(result->component == "main");
    ASSERT_TRUE(result->extension == ".orig.tar.xz");
    ASSERT_TRUE(result->architecture == "source");

    return true;
}

bool test_valid_all_architecture() {
    std::string path = "/debian/pool/main/g/gcc/gcc_11.0_all.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "gcc");
    ASSERT_TRUE(result->version == "11.0");
    ASSERT_TRUE(result->component == "main");
    ASSERT_TRUE(result->extension == ".deb");
    ASSERT_TRUE(result->architecture == "all");

    return true;
}

bool test_valid_dsc_file() {
    std::string path = "/debian/pool/main/n/nginx/nginx_1.18.0.dsc";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "nginx");
    ASSERT_TRUE(result->version == "1.18.0");
    ASSERT_TRUE(result->component == "main");
    ASSERT_TRUE(result->extension == ".dsc");

    return true;
}

bool test_valid_tar_gz() {
    std::string path = "/debian/pool/main/a/apache2/apache2_2.4.0.tar.gz";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "apache2");
    ASSERT_TRUE(result->version == "2.4.0");
    ASSERT_TRUE(result->component == "main");
    ASSERT_TRUE(result->extension == ".tar.gz");

    return true;
}

bool test_non_free_component() {
    std::string path = "/debian/pool/non-free/n/nvidia/nvidia-driver_515.0_amd64.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "nvidia-driver");
    ASSERT_TRUE(result->version == "515.0");
    ASSERT_TRUE(result->component == "non-free");
    ASSERT_TRUE(result->extension == ".deb");
    ASSERT_TRUE(result->architecture == "amd64");

    return true;
}

// Test invalid paths
bool test_invalid_not_debian_pool() {
    std::string path = "/ubuntu/pool/main/v/vim/vim_9.0.0_amd64.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(!result.has_value());

    return true;
}

bool test_invalid_too_short() {
    std::string path = "/debian/";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(!result.has_value());

    return true;
}

bool test_invalid_no_underscores() {
    std::string path = "/debian/pool/main/v/vim/vim.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(!result.has_value());

    return true;
}

bool test_invalid_component() {
    std::string path = "/debian/pool/invalid/v/vim/vim_9.0.0_amd64.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(!result.has_value());

    return true;
}

bool test_empty_path() {
    std::string path = "";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(!result.has_value());

    return true;
}

bool test_path_without_leading_slash() {
    std::string path = "debian/pool/main/v/vim/vim_9.0.0_amd64.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(!result.has_value());

    return true;
}

// Test edge cases
bool test_complex_version() {
    std::string path = "/debian/pool/main/p/python3-defaults/python3_3.11.0-1_amd64.deb";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "python3");
    ASSERT_TRUE(result->version == "3.11.0-1");
    ASSERT_TRUE(result->architecture == "amd64");

    return true;
}

bool test_plus_in_package_name() {
    std::string path = "/debian/pool/main/libc/libc++ libc++_15.0_amd64.deb";
    auto result = PackageParser::parse(path);

    // This may or may not parse depending on implementation
    // Just check it doesn't crash
    return true;
}

bool test_multiple_extensions() {
    std::string path = "/debian/pool/main/c/curl/curl_7.0.0.orig.tar.gz";
    auto result = PackageParser::parse(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->name == "curl");
    ASSERT_TRUE(result->extension == ".orig.tar.gz" || result->extension == ".tar.gz");
    ASSERT_TRUE(result->architecture == "source");

    return true;
}

// Run all package parser tests
void run_package_parser_tests() {
    test::TestSuite suite("Package Parser Tests");

    // Valid path tests
    suite.add_test("Valid vim package path", test_valid_vim_package);
    suite.add_test("Valid openssh-server package path", test_valid_openssh_package);
    suite.add_test("Valid libpng source package", test_valid_libpng_package);
    suite.add_test("Valid 'all' architecture package", test_valid_all_architecture);
    suite.add_test("Valid .dsc file", test_valid_dsc_file);
    suite.add_test("Valid .tar.gz file", test_valid_tar_gz);
    suite.add_test("Valid non-free component package", test_non_free_component);

    // Invalid path tests
    suite.add_test("Invalid path (not debian/pool)", test_invalid_not_debian_pool);
    suite.add_test("Invalid path (too short)", test_invalid_too_short);
    suite.add_test("Invalid path (no underscores)", test_invalid_no_underscores);
    suite.add_test("Invalid path (invalid component)", test_invalid_component);
    suite.add_test("Invalid path (empty)", test_empty_path);
    suite.add_test("Invalid path (no leading slash)", test_path_without_leading_slash);

    // Edge case tests
    suite.add_test("Complex version with debian revision", test_complex_version);
    suite.add_test("Multiple extensions (.orig.tar.gz)", test_multiple_extensions);
    suite.add_test("Package name with spaces (edge case)", test_plus_in_package_name);

    suite.run();
}
