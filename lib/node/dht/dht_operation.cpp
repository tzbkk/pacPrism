#include "node/dht/dht_operation.h"
#include <vector>

void dht_operation::store_entry(dht_entry entry) {
    dht_operation::stored_entries.push_back(std::move(entry));
}