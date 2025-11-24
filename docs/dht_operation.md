# dht_operation Class Documentation

The `dht_operation` class provides core functionality for managing Distributed Hash Table (DHT) entries in the pacPrism system. This class handles storage, querying, removal, and cleanup of DHT entries with automatic TTL (Time To Live) management.

## Class Overview

```cpp
class dht_operation {
    // ...
};
```

## Public Member Functions

### store_entry
```cpp
void store_entry(dht_entry entry);
```

**Description**: Stores a DHT entry in the local storage. If an entry with the same node IP already exists, it will be updated only if the new entry has a more recent timestamp.

**Parameters**:
- `entry`: The `dht_entry` object to store

**Behavior**:
- If no entry exists for the given node IP, a new entry is added
- If an entry already exists, it's only updated if the new entry's timestamp is more recent
- Automatically manages TTL entries for expiration tracking

### query_entry (by node IP)
```cpp
dht_entry query_entry(const std::string& node_ip) const;
```

**Description**: Retrieves a DHT entry by its node IP address.

**Parameters**:
- `node_ip`: The IP address of the node to query

**Returns**:
- The `dht_entry` object if found
- An empty `dht_entry` object if not found

### query_entry (by sharding)
```cpp
std::vector<dht_entry> query_entry(const sharding& sharding_querying) const;
```

**Description**: Retrieves all DHT entries that contain the specified sharding information.

**Parameters**:
- `sharding_querying`: The sharding object to search for

**Returns**:
- A vector of `dht_entry` objects that contain the specified sharding
- An empty vector if no matching entries are found

### remove_entry
```cpp
void remove_entry(const std::string& node_ip);
```

**Description**: Removes a DHT entry by its node IP address.

**Parameters**:
- `node_ip`: The IP address of the node to remove

**Behavior**:
- Removes the entry from all internal data structures
- Updates indices for remaining entries
- No effect if no entry with the given IP exists

### clean_by_ttl
```cpp
void clean_by_ttl();
```

**Description**: Removes all expired DHT entries based on their TTL values.

**Behavior**:
- Compares entry expiration times with the current system time
- Removes all entries that have expired
- Automatically called periodically to maintain DHT cleanliness

## Private Member Variables

### stored_entries
```cpp
std::vector<dht_entry> stored_entries;
```

**Description**: Vector containing all stored DHT entries.

### map_node_ip_to_stored_entries_index
```cpp
std::map<std::string, int> map_node_ip_to_stored_entries_index;
```

**Description**: Map for fast lookup of entry indices by node IP address.

### ttl_entries
```cpp
std::set<std::pair<int64_t, std::string>> ttl_entries;
```

**Description**: Set of TTL entries sorted by expiration time for efficient cleanup operations.

## Data Structures

### dht_entry
```cpp
struct dht_entry {
    std::string node_ip;           // Node IP address
    std::vector<sharding> node_sharding;  // Sharding information
    int64_t entry_timestamp;       // Entry creation/modification timestamp
    int64_t node_ttl;              // Time To Live in seconds
};
```

### sharding
```cpp
struct sharding {
    std::string sharding_id;              // Unique identifier for the shard
    std::vector<std::string> packages;    // List of packages in this shard
};
```

## Thread Safety

The `dht_operation` class is not thread-safe. External synchronization is required if accessed from multiple threads.

## Example Usage

```cpp
#include "node/dht/dht_operation.h"

// Create DHT operation instance
dht_operation dht_ops;

// Create and store an entry
dht_entry entry;
entry.node_ip = "192.168.1.100";
entry.entry_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
entry.node_ttl = 3600; // 1 hour

sharding shard;
shard.sharding_id = "shard-001";
shard.packages = {"package1", "package2"};
entry.node_sharding.push_back(shard);

dht_ops.store_entry(entry);

// Query the entry
dht_entry retrieved = dht_ops.query_entry("192.168.1.100");

// Clean expired entries
dht_ops.clean_by_ttl();
```