// =============================================================================
// pacPrism - DHT Operation Interface
// =============================================================================
//
// This header exports the interface for DHT (Distributed Hash Table) operations:
// - dht_operation::store_entry()    : Store DHT entries with TTL management
// - dht_operation::query_entry()    : Query entries by node IP or sharding
// - dht_operation::remove_entry()   : Remove entries by node IP
// - dht_operation::clean_by_ttl()   : Clean expired entries based on TTL
//
// =============================================================================

#ifndef DHT_OPERATION_H
#define DHT_OPERATION_H

#include "node/dht/dht_types.h"
#include <vector>
#include <map>
#include <set>

// =============================================================================
// dht_operation Class
// =============================================================================
// Provides core functionality for managing Distributed Hash Table entries
// in the pacPrism system with automatic TTL (Time To Live) management.
// =============================================================================

class dht_operation {
    private:
        // Vector containing all stored DHT entries
        std::vector<dht_entry> stored_entries;

        // Map for fast lookup of entry indices by node IP address
        std::map<std::string, int> map_node_ip_to_stored_entries_index;

        // Set of TTL entries sorted by expiration time for efficient cleanup
        std::set<std::pair<int64_t, std::string>> ttl_entries;

    public:
        // -------------------------------------------------------------------------
        // Storage Operations
        // -------------------------------------------------------------------------

        // Store a DHT entry. If an entry with the same node IP already exists,
        // it will be updated only if the new entry has a more recent timestamp.
        void store_entry(dht_entry entry);

        // -------------------------------------------------------------------------
        // Query Operations
        // -------------------------------------------------------------------------

        // Query a DHT entry by node IP address
        dht_entry query_entry(const std::string& node_ip) const;

        // Query DHT entries by sharding information
        std::vector<dht_entry> query_entry(const sharding& sharding_querying) const;

        // -------------------------------------------------------------------------
        // Removal Operations
        // -------------------------------------------------------------------------

        // Remove a DHT entry by node IP address
        void remove_entry(const std::string& node_ip);

        // -------------------------------------------------------------------------
        // Maintenance Operations
        // -------------------------------------------------------------------------

        // Remove all expired DHT entries based on their TTL values
        void clean_by_ttl();
};

#endif // DHT_OPERATION_H