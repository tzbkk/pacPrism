// This head defines necessary types & structs for DHT:
// - struct dht_entry
#pragma once

#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <set>

#include <node/sharding/sharding_types.hpp>

// struct dht_entry: entry for DHT,
// including a node's ip, sharding,
// timestamp and TTL.
struct dht_entry {
    std::string node_id;
    std::string node_ip;
    std::set<shard> node_shard;
    int64_t generation_timestamp;
    int64_t expiry_timestamp;
    std::string information;
};