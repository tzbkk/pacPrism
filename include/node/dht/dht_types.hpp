// This head defines necessary types & structs for DHT:
// - struct dht_entry
#pragma once

#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <set>

#include <node/sharding/sharding_types.hpp>
#include <nlohmann/json.hpp>

// JSON serialization for shard
namespace nlohmann {
    template <>
    struct adl_serializer<shard> {
        static void to_json(json& j, const shard& s) {
            j = json{
                {"shard_id", s.shard_id},
                {"packages", s.packages}
            };
        }

        static void from_json(const json& j, shard& s) {
            j.at("shard_id").get_to(s.shard_id);
            j.at("packages").get_to(s.packages);
        }
    };
}

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

// JSON serialization for dht_entry
namespace nlohmann {
    template <>
    struct adl_serializer<dht_entry> {
        static void to_json(json& j, const dht_entry& e) {
            j = json{
                {"node_id", e.node_id},
                {"node_ip", e.node_ip},
                {"node_shard", e.node_shard},
                {"generation_timestamp", e.generation_timestamp},
                {"expiry_timestamp", e.expiry_timestamp},
                {"information", e.information}
            };
        }

        static void from_json(const json& j, dht_entry& e) {
            j.at("node_id").get_to(e.node_id);
            j.at("node_ip").get_to(e.node_ip);
            j.at("node_shard").get_to(e.node_shard);
            j.at("generation_timestamp").get_to(e.generation_timestamp);
            j.at("expiry_timestamp").get_to(e.expiry_timestamp);
            j.at("information").get_to(e.information);
        }
    };
}