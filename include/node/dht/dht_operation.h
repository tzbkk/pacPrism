// DHT operations interface for pacPrism distributed hash table

#ifndef DHT_OPERATION_H
#define DHT_OPERATION_H

#include <node/dht/dht_types.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <optional>

// DHT operation class for managing distributed hash table entries
class dht_operation {
private:
    // Where entries are stored.
    std::unordered_map<std::string, dht_entry> stored_entries;
    // Timestamp and IP.
    std::set<std::pair<int64_t, std::string>> ttl_entries;

public:
    // Store entry to the vector stored_entries.
    void store_entry(dht_entry entry);

    // Query an entry by node_ip.
    std::optional<dht_entry> query_entry(const std::string& node_ip) const;

    // Query entries by sharding.
    std::vector<dht_entry> query_entry(const sharding& sharding_querying) const;

    // Remove expired entries based on TTL.
    void clean_by_ttl();

private:
    // The function can only be called by clean_by_ttl.
    // Remove DHT entry by node IP address.
    void remove_entry(const std::string& node_ip);
};

#endif // DHT_OPERATION_H