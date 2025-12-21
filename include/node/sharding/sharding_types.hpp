// This head defines types for sharding:
// - struct sharding
#pragma once

#include <vector>
#include <iostream>
#include <string>

// struct sharding: an sharding entry including
// its id and packages linked.
struct shard {
    std::string shard_id;
    std::vector<std::string> packages;
};