#include "node/dht/dht_operation.h"
#include <vector>

// Store entry to the vector stored_entries.
void dht_operation::store_entry(dht_entry entry) {
    // Whether the entry exist. If not, store a new entry.
    if (map_node_ip_to_stored_entries_index.find(entry.node_ip) == map_node_ip_to_stored_entries_index.end()) {
        // Update map.
        map_node_ip_to_stored_entries_index[entry.node_ip] = stored_entries.size() - 1;
        // Push the new entry.
        dht_operation::stored_entries.push_back(std::move(entry));
    }
    // If exist, check the timestamp. Store the newer one.
    else {
        // Get the index of the entry.
        int stored_entry_index = map_node_ip_to_stored_entries_index[entry.node_ip];
        // Check the timestamp and update.
        if (stored_entries[stored_entry_index].entry_timestamp < entry.entry_timestamp) {
            stored_entries[stored_entry_index].node_sharding = entry.node_sharding;
            stored_entries[stored_entry_index].entry_timestamp = entry.entry_timestamp;
            stored_entries[stored_entry_index].node_ttl = entry.node_ttl;
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

}

// Remove entry from the vector stored_entries.
void dht_operation::remove_entry(std::string node_ip) {
    
}