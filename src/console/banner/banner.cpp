#include <iostream>

#include <console/banner/banner.hpp>
#include <pacPrism/version.h>

void Banner::print() const {
    std::cout << "\033[32mpacPrism - Semi-decentralized Package Distribution System\033[0m" << std::endl;
    std::cout << "Version \033[34m" << pacprism::getVersionFull() << "\033[0m" << std::endl;
    std::cout << "Build \033[33m" << pacprism::getBuildInfo() << "\033[0m" << std::endl;
    if (pacprism::getGitInfo() != "no git info") {
        std::cout << "Git: \033[35m" << pacprism::getGitInfo() << "\033[0m" << std::endl;
    }
}