// This head defines necessary types & structs for DHT:
// - struct dht_entry
#pragma once

#include <node/sharding/sharding_types.hpp>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <string>

// struct dht_entry: entry for DHT,
// including a node's ip, sharding,
// timestamp and TTL.
struct dht_entry {
    std::string node_ip;
    std::unordered_map<std::string, sharding> node_sharding;
    int64_t entry_timestamp;
    int64_t node_ttl;
};