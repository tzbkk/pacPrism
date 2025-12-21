#include <vector>
#include <chrono>
#include <optional>

#include <node/dht/dht_operation.hpp>

bool DHT_operation::verify_entry(std::string node_id) {
    if (node_id_to_generation_timestamp_entries.contains(node_id)) {
        return true;
    }
    return false;
}

void DHT_operation::store_entry(dht_entry entry) {
    if (verify_entry(entry.node_id)) {
        if (node_id_to_generation_timestamp_entries[entry.node_id] < entry.generation_timestamp) this->remove_entry(entry.node_id);
        else return;
    }
    node_ip_to_node_id_entries[entry.node_ip] = entry.node_id;
    node_id_to_node_ip_entries[entry.node_id] = entry.node_ip;
    node_id_to_generation_timestamp_entries[entry.node_id] = entry.generation_timestamp;
    expiry_timestamp_to_node_id_entries.insert({entry.expiry_timestamp, entry.node_id});
    node_id_to_expiry_timestamp_entries[entry.node_id] = entry.expiry_timestamp;
    for (auto shard : entry.node_shard) {
        shard_id_to_node_ids_entries[shard.shard_id].insert(entry.node_id);
        node_id_to_shard_ids_entries[entry.node_id].insert(shard.shard_id);
    }
    node_id_to_information_entries[entry.node_id] = entry.information;
    node_id_to_liveness_entries[entry.node_id] = 0;
}

const std::set<std::string>* DHT_operation::query_node_ids_by_shard_id(std::string shard_id_querying) {
    if (shard_id_to_node_ids_entries.contains(shard_id_querying)) {
        return &shard_id_to_node_ids_entries[shard_id_querying];
    }
    return nullptr;
}

void DHT_operation::remove_entry(const std::string& node_id) {
    if (verify_entry(node_id) == false) return;
    const std::string& node_ip = node_id_to_node_ip_entries[node_id];
    if (node_ip_to_node_id_entries[node_ip] == node_id) {
        node_ip_to_node_id_entries.erase(node_ip);
    }
    node_id_to_node_ip_entries.erase(node_id);
    node_id_to_generation_timestamp_entries.erase(node_id);
    for (auto shard_id : node_id_to_shard_ids_entries[node_id]) {
        shard_id_to_node_ids_entries[shard_id].erase(node_id);
    }
    expiry_timestamp_to_node_id_entries.erase({node_id_to_expiry_timestamp_entries[node_id], node_id});
    node_id_to_expiry_timestamp_entries.erase(node_id);
    node_id_to_shard_ids_entries.erase(node_id);
    node_id_to_information_entries.erase(node_id);
    node_id_to_liveness_entries.erase(node_id);
}

void DHT_operation::clean_by_expiry_time() {
    // Get current timestamp
    auto now_sec = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    // Record entries to remove.
    std::vector<std::string> entries_to_remove;
    auto it = expiry_timestamp_to_node_id_entries.begin();
    while(it != expiry_timestamp_to_node_id_entries.end()) {
        if (it->first <= now_sec) {
            entries_to_remove.push_back(it->second);
            it++;
        } else {
            break;
        }
    }
    // Remove expired entries and pairs.
    for (auto node_id : entries_to_remove) {
        remove_entry(node_id);
    }
}