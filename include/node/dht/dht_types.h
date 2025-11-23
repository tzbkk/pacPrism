// This head defines necessary types & structs for DHT:
// - struct dht_entry

#ifndef DHT_TYPES_H
#define DHT_TYPES_H

#include <node/sharding/sharding_types.h>
#include <iostream>
#include <cstdint>
#include <vector>

// struct dht_entry: entry for DHT, 
// including a node's ip, sharding,
// timestamp and TTL.
struct dht_entry {
    std::string node_ip;
    std::vector<sharding> node_sharding;
    int64_t entry_timestamp;
    int64_t node_ttl;
};

#endif