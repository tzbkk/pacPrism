# DHT_operation Class Documentation

> **Implementation Status**: Single-process in-memory prototype only. Not a true distributed system.

The `DHT_operation` class provides a **single-process in-memory** hash-based index with multi-dimensional lookups. Despite the name "DHT", this is **not a distributed hash table** - it's a local data structure with O(1) lookup performance.

## Current Reality

- ✅ **Works**: In-memory CRUD operations
- ✅ **Works**: Multi-dimensional indexing via multiple unordered_maps
- ❌ **Not Distributed**: Single-process only, no network communication
- ❌ **No Persistence**: Data lost on process restart
- ❌ **No Replication**: No data copies across nodes

## Architecture Overview

```cpp
class DHT_operation {
    // 9-dimensional indexing system for optimal performance
    // Node ID-based architecture for distributed computing
    // Complete RAII and exception safety (C++23)
};
```

## Design Principles

### Node ID-Based Architecture
- **Primary Identifier**: Node IDs instead of IP addresses
- **Flexible Identification**: Supports IPv4, IPv6, hostnames, or any string identifier
- **Future-Proof**: Designed for P2P network integration
- **Abstraction**: Clean separation between network identity and DHT operations

### Multi-Dimensional Indexing
Nine separate index structures provide O(1) performance for different query patterns:
- **Identity Index**: IP↔ID bidirectional mappings
- **Temporal Index**: Dual timestamp indexing (generation + expiry)
- **Spatial Index**: Shard-based package distribution
- **Health Index**: Liveness and metadata tracking

## Public Member Functions

### verify_entry
```cpp
bool verify_entry(const std::string& node_id);
```

**Description**: Fast validation of node existence without full entry retrieval.

**Parameters**:
- `node_id`: The node identifier to verify

**Returns**:
- `true` if the node exists in the DHT
- `false` if the node is not found

**Performance**: O(1) average time complexity

**Use Cases**:
- Quick existence checks before operations
- Route validation in distributed systems
- Health monitoring without data retrieval

### store_entry
```cpp
void store_entry(dht_entry entry);
```

**Description**: Stores or updates a DHT entry with intelligent conflict resolution based on generation timestamps.

**Parameters**:
- `entry`: The `dht_entry` object to store

**Behavior**:
- If the node doesn't exist: Creates a new entry with full indexing
- If the node exists: Updates only if new entry has newer generation timestamp
- Automatically updates all 9 index structures
- Manages shard relationships and TTL entries

**Performance**: O(k) where k is the number of shards in the entry

**Conflict Resolution**:
- Timestamp-based conflict resolution
- Preserves data consistency across all indexes
- Atomic updates prevent partial states

### query_node_ids_by_shard_id
```cpp
const std::set<std::string>* query_node_ids_by_shard_id(const std::string& shard_id);
```

**Description**: Retrieves all node IDs that contain a specific shard.

**Parameters**:
- `shard_id`: The shard identifier to query

**Returns**:
- Pointer to `std::set<std::string>` containing matching node IDs
- `nullptr` if no nodes contain the shard

**Performance**: O(1) average time complexity

**Memory Safety**: Returns const pointer to internal data, caller should not modify

**Use Cases**:
- Package distribution routing
- Load balancing across nodes
- Shard availability queries

### entry_builder
```cpp
dht_entry entry_builder(const std::string& node_id);
```

**Description**: Constructs a new DHT entry for a given node ID with pre-populated fields.

**Parameters**:
- `node_id`: The node identifier for the new entry

**Returns**:
- `dht_entry` object with node_id populated

**Status**: Declared, implementation pending

**Planned Features**:
- Automatic timestamp generation
- Default TTL assignment
- Validation of node_id format

### clean_by_expiry_time
```cpp
void clean_by_expiry_time();
```

**Description**: Removes all expired entries based on their expiry timestamps.

**Behavior**:
- Iterates through expiry timestamp index in sorted order
- Efficiently stops when non-expired entries are encountered
- Removes entries from all 9 index structures
- Batch processing for performance optimization

**Performance**: O(n) where n is the number of expired entries

**Cleanup Strategy**:
- Two-pass approach: identify, then remove
- Atomic removal prevents partial states
- Maintains index consistency

### clean_by_liveness
```cpp
void clean_by_liveness();
```

**Description**: Removes unhealthy nodes based on liveness tracking metrics.

**Status**: Declared, implementation pending

**Planned Features**:
- Health score calculation
- Configurable liveness thresholds
- Graceful degradation strategies

## Private Member Functions

### remove_entry
```cpp
void remove_entry(const std::string& node_id);
```

**Description**: Complete removal of a node from all 9 index structures.

**Parameters**:
- `node_id`: The node identifier to remove

**Access**: Private (can only be called by cleanup functions)

**Behavior**:
- Removes from all bidirectional mappings
- Cleans up timestamp entries
- Updates shard relationships
- Removes metadata and liveness data

**Atomic Operations**: Ensures all indexes remain consistent

## Private Member Variables (9-Dimensional Index System)

### Identity Indexes
```cpp
std::unordered_map<std::string, std::string> node_ip_to_node_id_entries;
std::unordered_map<std::string, std::string> node_id_to_node_ip_entries;
```
**Description**: Bidirectional mappings between IP addresses and node IDs for flexible node identification.

### Temporal Indexes
```cpp
std::unordered_map<std::string, int64_t> node_id_to_generation_timestamp_entries;
std::unordered_map<std::string, int64_t> node_id_to_expiry_timestamp_entries;
std::set<std::pair<int64_t, std::string>> expiry_timestamp_to_node_id_entries;
```
**Description**: Dual timestamp indexing for lifecycle management and efficient cleanup operations.

### Spatial Indexes
```cpp
std::unordered_map<std::string, std::set<std::string>> shard_id_to_node_ids_entries;
std::unordered_map<std::string, std::set<std::string>> node_id_to_shard_ids_entries;
```
**Description**: Bidirectional shard relationships for package distribution queries.

### Health Indexes
```cpp
std::unordered_map<std::string, std::string> node_id_to_information_entries;
std::unordered_map<std::string, int> node_id_to_liveness_entries;
```
**Description**: Node metadata and health tracking for distributed system monitoring.

## Data Structures

### dht_entry (Node ID-based Architecture)
```cpp
struct dht_entry {
    std::string node_id;                    // Primary node identifier
    std::string node_ip;                    // Network address (optional)
    std::set<shard> node_shard;            // Set of shards associated with node
    int64_t generation_timestamp;          // Entry creation/update time
    int64_t expiry_timestamp;              // Entry expiration time
    std::string information;               // Extensible metadata field
};
```

**Key Changes from Previous Architecture**:
- `node_id` added as primary identifier
- `node_shard` changed from map to set for uniqueness
- Timestamps renamed for clarity (generation vs expiry)
- Added `information` field for extensibility

### shard (Simplified Structure)
```cpp
struct shard {
    std::string shard_id;                  // Unique shard identifier
    std::vector<std::string> packages;     // List of packages in shard
};
```

**Naming Update**: Changed from `sharding` to `shard` for consistency and brevity.

## Performance Characteristics

### Time Complexity
| Operation | Complexity | Notes |
|-----------|------------|-------|
| `verify_entry()` | O(1) | Hash map lookup |
| `store_entry()` | O(k) | k = number of shards |
| `query_node_ids_by_shard_id()` | O(1) | Direct hash map access |
| `clean_by_expiry_time()` | O(n) | n = expired entries only |
| `remove_entry()` | O(k) | k = number of associated shards |

### Memory Usage
- **Base Overhead**: 9 separate hash maps and sets
- **Per Node**: O(k) where k is number of shards
- **Indexing Overhead**: ~4x base data size for multi-dimensional access
- **Trade-off**: Memory for O(1) query performance

## Thread Safety

The `DHT_operation` class is **not thread-safe**. External synchronization is required if accessed from multiple threads. For high-concurrency scenarios, consider:

- Reader-writer locks for read-heavy workloads
- Thread-local instances for independent operations
- Concurrent data structures for future scalability

## Error Handling

### Exception Safety
- **Basic Guarantee**: No resource leaks on exceptions
- **Strong Guarantee**: Operations are atomic or have no effect
- **No-throw Guarantee**: Query operations don't throw exceptions

### Validation
- Node ID format validation (planned)
- Shard identifier consistency checks
- Timestamp range validation

## Example Usage

```cpp
#include "node/dht/dht_operation.hpp"

// Create DHT operation instance
DHT_operation dht_ops;

// Create a new node entry
dht_entry entry;
entry.node_id = "node-001";
entry.node_ip = "192.168.1.100";
entry.generation_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
entry.expiry_timestamp = entry.generation_timestamp + 3600; // 1 hour TTL
entry.information = "region: us-west-1; capacity: high";

// Add shards to the node
shard shard1;
shard1.shard_id = "web-server";
shard1.packages = {"nginx", "apache", "lighttpd"};

shard shard2;
shard2.shard_id = "database";
shard2.packages = {"mysql", "postgresql"};

entry.node_shard.insert(shard1);
entry.node_shard.insert(shard2);

// Store the entry
dht_ops.store_entry(entry);

// Verify node exists
if (dht_ops.verify_entry("node-001")) {
    std::cout << "Node exists in DHT" << std::endl;
}

// Query nodes by shard
auto web_server_nodes = dht_ops.query_node_ids_by_shard_id("web-server");
if (web_server_nodes) {
    std::cout << "Found " << web_server_nodes->size() << " web server nodes" << std::endl;
    for (const auto& node_id : *web_server_nodes) {
        std::cout << "  - " << node_id << std::endl;
    }
}

// Clean expired entries
dht_ops.clean_by_expiry_time();
```

## Integration Patterns

### HTTP API Integration
```cpp
// Router can use DHT for package-aware routing
class Router {
    DHT_operation& dht;
public:
    Router(DHT_operation& dht_ref) : dht(dht_ref) {}

    std::vector<std::string> find_nodes_for_package(const std::string& package) {
        // Query DHT to find nodes containing packages in their shards
        // Implementation depends on shard-to-package mapping strategy
    }
};
```

### Dependency Injection
```cpp
// Main application setup
DHT_operation dht;
Router router(dht);  // Dependency injection for loose coupling
ServerTrans server(io_context, router);
```

## Migration Guide

### From IP-based DHT (Previous Architecture)
1. **Node Identification**: Replace IP-based lookups with Node ID-based lookups
2. **Query Methods**: Update from `query_entry(ip)` to `query_node_ids_by_shard_id(shard_id)`
3. **Data Structures**: Update `dht_entry` structure to include `node_id`
4. **Cleanup**: Replace `clean_by_ttl()` with `clean_by_expiry_time()`

### Performance Considerations
- **Migration Cost**: O(n) to rebuild indexes from existing data
- **Memory Impact**: ~4x increase due to multi-dimensional indexing
- **Query Performance**: Significant improvement for shard-based queries
- **Flexibility Gain**: Support for non-IP node identification

## Future Enhancements

### Planned Features
- **P2P Integration**: Node discovery and network bootstrapping
- **Replication**: Multi-node data consistency and synchronization
- **Persistence**: Database backend integration for durability
- **Metrics**: Performance monitoring and query analytics
- **Sharding Strategy**: Intelligent package-to-shard mapping algorithms

### Scalability Considerations
- **Distributed Queries**: Cross-node shard queries
- **Load Balancing**: Intelligent node selection based on health
- **Data Partitioning**: Geographic or logical DHT partitioning
- **Cache Layers**: Multi-level caching for hot data access

This architecture provides a solid foundation for scalable distributed package management with high query performance and flexible node identification suitable for modern cloud-native environments.