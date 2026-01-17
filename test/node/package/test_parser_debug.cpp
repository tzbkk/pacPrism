#include "../../common.hpp"
#include <node/package/parser.hpp>
#include <iostream>

bool test_debug_parser() {
    std::string path = "/debian/pool/main/v/vim/vim_9.0.0_amd64.deb";
    std::cout << "Testing path: " << path << std::endl;

    auto result = PackageParser::parse(path);

    if (result.has_value()) {
        std::cout << "  Parsed successfully!" << std::endl;
        std::cout << "  name: " << result->name << std::endl;
        std::cout << "  version: " << result->version << std::endl;
        std::cout << "  component: " << result->component << std::endl;
        std::cout << "  extension: " << result->extension << std::endl;
        std::cout << "  architecture: " << result->architecture << std::endl;
    } else {
        std::cout << "  FAILED: parse() returned nullopt" << std::endl;
    }

    return result.has_value();
}

int main() {
    test_debug_parser();
    return 0;
}
