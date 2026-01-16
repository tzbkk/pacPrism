#pragma once

#include <string>

// Banner printer for pacPrism
class Banner {
public:
    Banner() = default;

    // Print the application banner with version information
    void print() const;
};