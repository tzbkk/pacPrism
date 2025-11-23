// This head exports interface to call DHT_operation related functions:
// dht_operation::store_entry()
// dht_operation::query_entry()
// dht_operation::remove_entry()
// dht_operation::clean_by_ttl()
// dht_operation::stored_entries
// dht_operation::map_node_ip_to_stored_entries_index
// dht_operation::ttl_entries

#ifndef DHT_OPERATION_H
#define DHT_OPERATION_H

#include "node/dht/dht_types.h"
#include <vector>
#include <map>
#include <set>

class dht_operation {
    private:
        std::vector<dht_entry> stored_entries;
        std::map<std::string, int> map_node_ip_to_stored_entries_index;
        std::set<std::pair<int64_t, std::string>> ttl_entries;
    public:
        void store_entry(dht_entry entry);
        dht_entry query_entry(const std::string& node_ip) const;
        std::vector<dht_entry> query_entry(const sharding& sharding_querying) const;
        void remove_entry(const std::string& node_ip);
        void clean_by_ttl();
};

#endif