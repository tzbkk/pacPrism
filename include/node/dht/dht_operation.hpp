// DHT operations interface for pacPrism distributed hash table
#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <optional>
#include <cstdint>

#include <node/dht/dht_types.hpp>

// DHT operation class for managing distributed hash table entries
class DHT_operation {
private:
    // Node IP to node ID.
    std::unordered_map<std::string, std::string> node_ip_to_node_id_entries;
    // Node ID to node IP.
    std::unordered_map<std::string, std::string> node_id_to_node_ip_entries;
    // Node ID to generation timestamp.
    std::unordered_map<std::string, int64_t> node_id_to_generation_timestamp_entries;
    // Expiry timestamp to node ID.
    std::set<std::pair<int64_t, std::string>> expiry_timestamp_to_node_id_entries;
    // Node ID to expiry timestamp.
    std::unordered_map<std::string, int64_t> node_id_to_expiry_timestamp_entries;
    // Shard ID to node IDs.
    std::unordered_map<std::string, std::set<std::string>> shard_id_to_node_ids_entries;
    // Node ID to shard IDs.
    std::unordered_map<std::string, std::set<std::string>> node_id_to_shard_ids_entries;
    // Node ID to information.
    std::unordered_map<std::string, std::string> node_id_to_information_entries;
    // Node ID to liveness.
    std::unordered_map<std::string, int> node_id_to_liveness_entries;

public:
    // Check if an entry exist.
    bool verify_entry(std::string node_id);
    // Store an entry.
    void store_entry(dht_entry entry);
    // Query node IDs by shard ID.
    const std::set<std::string>* query_node_ids_by_shard_id(std::string shard_id_querying);
    // Build an entry.
    dht_entry entry_builder(std::string node_id);
    // Remove expired nodes based on expiry time.
    void clean_by_expiry_time();
    // Remove unhealthy nodes based on liveness.
    void clean_by_liveness();

private:
    // Remove a node by ID from all entries.
    void remove_entry(const std::string& node_id);
};