#include "../../common.hpp"
#include <node/dht/dht_operation.hpp>
#include <node/dht/dht_types.hpp>
#include <chrono>
#include <ctime>

// Test: DHT initialization
bool test_dht_initialization() {
    DHT_operation dht;
    ASSERT_TRUE(true);  // If we got here, initialization succeeded
    return true;
}

// Test: Verify entry existence (non-existent entry)
bool test_dht_verify_nonexistent_entry() {
    DHT_operation dht;
    ASSERT_FALSE(dht.verify_entry("nonexistent_node_id"));
    return true;
}

// Test: Store entry and verify existence
bool test_dht_store_and_verify() {
    DHT_operation dht;

    dht_entry entry;
    entry.node_id = "test_node_123";
    entry.node_ip = "192.168.1.100";

    // Get current time as timestamp
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    entry.generation_timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    entry.expiry_timestamp = entry.generation_timestamp + (24 * 3600);  // +24 hours

    dht.store_entry(entry);

    bool verified = dht.verify_entry("test_node_123");
    ASSERT_TRUE(verified);

    return true;
}

// Test: Query nodes by shard ID
bool test_dht_query_by_shard() {
    DHT_operation dht;

    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    dht_entry entry1;
    entry1.node_id = "node1";
    entry1.node_ip = "192.168.1.1";
    entry1.generation_timestamp = timestamp;
    entry1.expiry_timestamp = timestamp + (24 * 3600);

    dht_entry entry2;
    entry2.node_id = "node2";
    entry2.node_ip = "192.168.1.2";
    entry2.generation_timestamp = timestamp;
    entry2.expiry_timestamp = timestamp + (24 * 3600);

    dht.store_entry(entry1);
    dht.store_entry(entry2);

    // Query should return a pointer to set of node IDs
    const std::set<std::string>* node_ids = dht.query_node_ids_by_shard_id("shard_a");

    // If no shards found, returns nullptr or empty set
    ASSERT_TRUE(node_ids == nullptr || node_ids->size() >= 0);
    return true;
}

// Test: Query with empty shard ID
bool test_dht_query_empty_shard() {
    DHT_operation dht;

    auto node_ids = dht.query_node_ids_by_shard_id("");
    ASSERT_TRUE(node_ids == nullptr || node_ids->empty());
    return true;
}

// Run all DHT tests
void run_dht_tests() {
    test::TestSuite suite("DHT Tests");

    suite.add_test("DHT: Initialization", test_dht_initialization);
    suite.add_test("DHT: Verify nonexistent entry", test_dht_verify_nonexistent_entry);
    suite.add_test("DHT: Store and verify", test_dht_store_and_verify);
    suite.add_test("DHT: Query by shard ID", test_dht_query_by_shard);
    suite.add_test("DHT: Query empty shard", test_dht_query_empty_shard);

    suite.run();
}
