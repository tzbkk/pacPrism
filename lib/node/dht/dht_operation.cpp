#include <node/dht/dht_operation.h>
#include <vector>
#include <chrono>
#include <optional>

void dht_operation::store_entry(dht_entry entry) {
    // Whether the entry exist. If not, store a new entry. Else update.
    if (stored_entries.find(entry.node_ip) == stored_entries.end()) {
        ttl_entries.insert({entry.entry_timestamp + entry.node_ttl, entry.node_ip});
        stored_entries[entry.node_ip] = std::move(entry);
    } else {
        if (stored_entries[entry.node_ip].entry_timestamp <= entry.entry_timestamp) {
            // Update old ttl entry.
            ttl_entries.erase({stored_entries[entry.node_ip].entry_timestamp + stored_entries[entry.node_ip].node_ttl, entry.node_ip});
            ttl_entries.insert({entry.entry_timestamp + entry.node_ttl, entry.node_ip});
            this->stored_entries[entry.node_ip] = std::move(entry);
        }
    }
}

std::optional<dht_entry> dht_operation::query_entry(const std::string& node_ip) const {
    auto query_result = stored_entries.find(node_ip);

    if (query_result != stored_entries.end()) {
        return query_result->second;
    }

    return std::nullopt;
}

std::vector<dht_entry> dht_operation::query_entry(const sharding& sharding_querying) const {
    std::vector<dht_entry> result;

    // Iterate through all stored entries
    for (const auto& entry : stored_entries) {
        // Check if this entry contains the queried sharding
        if (entry.second.node_sharding.find(sharding_querying.sharding_id) != entry.second.node_sharding.end()) {
            result.push_back(entry.second);
        }
    }

    return result;
}

void dht_operation::remove_entry(const std::string& node_ip) {
    // For only dht_operation::clean_by_ttl can remove an entry,
    // we need not to check if the entry exists again.
    stored_entries.erase(node_ip);
}

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