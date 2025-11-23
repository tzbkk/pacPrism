// This head exports interface to call DHT_operation related functions:
// dht_operation::store_entry()
// dht_operation::stored_entries

#ifndef DHT_OPERATION_H
#define DHT_OPERATION_H

#include "node/dht/dht_types.h"
#include <vector>

class dht_operation {
    private:
        std::vector<dht_entry> stored_entries;
    public:
        void store_entry(dht_entry entry);
};

#endif