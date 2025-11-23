#include "node/dht/dht_operation.h"
#include <vector>
#include <chrono>

// Store entry to the vector stored_entries.
void dht_operation::store_entry(dht_entry entry) {
    // Whether the entry exist. If not, store a new entry.
    if (map_node_ip_to_stored_entries_index.find(entry.node_ip) == map_node_ip_to_stored_entries_index.end()) {
        // Push the new entry first.
        dht_operation::stored_entries.push_back(std::move(entry));
        // Update map with correct index.
        map_node_ip_to_stored_entries_index[entry.node_ip] = static_cast<int>(stored_entries.size() - 1);
        // Update pair.
        ttl_entries.insert({entry.entry_timestamp + entry.node_ttl, entry.node_ip});
    }

    // If exist, check the timestamp. Store the newer one.
    else {
        // Get the index of the entry.
        int stored_entry_index = map_node_ip_to_stored_entries_index[entry.node_ip];
        // Check the timestamp and update.
        if (stored_entries[stored_entry_index].entry_timestamp < entry.entry_timestamp) {
            // Remove old TTL entry
            ttl_entries.erase({stored_entries[stored_entry_index].entry_timestamp + stored_entries[stored_entry_index].node_ttl, stored_entries[stored_entry_index].node_ip});
            // Update stored entry directly
            stored_entries[stored_entry_index].node_sharding = entry.node_sharding;
            stored_entries[stored_entry_index].entry_timestamp = entry.entry_timestamp;
            stored_entries[stored_entry_index].node_ttl = entry.node_ttl;
            // Add new TTL entry
            ttl_entries.insert({entry.entry_timestamp + entry.node_ttl, entry.node_ip});
        }
    }
}

// Query an entry by node_ip.
dht_entry dht_operation::query_entry(const std::string& node_ip) const {
    auto query_result = map_node_ip_to_stored_entries_index.find(node_ip);

    if (query_result != map_node_ip_to_stored_entries_index.end()) {
        return stored_entries[query_result->second];
    }

    return dht_entry{};
}

// Query entries by sharding.
std::vector<dht_entry> dht_operation::query_entry(const sharding& sharding_querying) const {
    std::vector<dht_entry> result;

    // Iterate through all stored entries
    for (const auto& entry : stored_entries) {
        // Check if this entry contains the queried sharding
        for (const auto& shard : entry.node_sharding) {
            if (shard.sharding_id == sharding_querying.sharding_id) {
                result.push_back(entry);
                break; // No need to check other shards in this entry
            }
        }
    }

    return result;
}

// Remove entry from the vector stored_entries.
void dht_operation::remove_entry(const std::string& node_ip) {
    // Query the index to remove.
    auto query_result = map_node_ip_to_stored_entries_index.find(node_ip);

    // Remove if exist.
    if (query_result != map_node_ip_to_stored_entries_index.end()) {
        int index_to_remove = query_result->second;

        // Remove from ttl_entries
        for (auto it = ttl_entries.begin(); it != ttl_entries.end(); ) {
            if (it->second == node_ip) {
                it = ttl_entries.erase(it);
            } else {
                ++it;
            }
        }

        // Remove from stored_entries
        stored_entries.erase(stored_entries.begin() + index_to_remove);

        // Remove from map
        map_node_ip_to_stored_entries_index.erase(node_ip);

        // Update indices in map for entries after the removed one
        for (auto& pair : map_node_ip_to_stored_entries_index) {
            if (pair.second > index_to_remove) {
                pair.second--;
            }
        }
    }
}

// Clean expired DHT entry.
void dht_operation::clean_by_ttl() {
    // Get current timestamp
    auto now_sec = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    // Remove expired entries and pairs.
    auto it = ttl_entries.begin();
    while (it != ttl_entries.end()) {
        if (it->first <= now_sec) {
            dht_operation::remove_entry(it->second);
            it = ttl_entries.erase(it);
        } else {
            break; // set is sorted, remaining entries are not expired
        }
    }
}