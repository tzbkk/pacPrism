// This head defines types for sharding:
// - struct sharding
#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

// struct sharding: an sharding entry including
// its id and packages linked.
struct shard {
    std::string shard_id;
    std::vector<std::string> packages;

    // Comparison operator for std::set ordering
    bool operator<(const shard& other) const {
        return shard_id < other.shard_id;
    }

    // Equality operator
    bool operator==(const shard& other) const {
        return shard_id == other.shard_id;
    }
};