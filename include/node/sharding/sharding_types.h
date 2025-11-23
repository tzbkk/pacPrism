// This head defines types for sharding:
// - struct sharding

#ifndef SHARDING_TYPE_H
#define SHARDING_TYPE_H

#include <vector>
#include <iostream>

// struct sharding: an sharding entry including
// its id and packages linked.
struct sharding {
    std::string sharding_id;
    std::vector<std::string> packages;
};

#endif