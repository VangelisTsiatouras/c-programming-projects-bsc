#ifndef BFSTORAGE_H
#define BFSTORAGE_H

#include <pthread.h>
#include "../bloomfilter/bloomfilter.h"

pthread_mutex_t storageMutex;

typedef struct BloomFilterArrayElement {
    BloomFilter *bloomFilter;
    int bloomFilterID;
    char bloomFilterObtained;
} BloomFilterArrayElement;

typedef struct BFStorage {
    BloomFilterArrayElement *bloomFilterArray;
    int numberOfBloomFilters;
} BFStorage;


BFStorage *create_bf_storage(int numberOfBloomFilters);
void destroy_bf_storage(BFStorage *bfStorage);
BloomFilterArrayElement *obtain_filter_bf_storage(BFStorage *bfStorage);
void release_filter_bf_storage(BFStorage *bfStorage, int position);

void *tester_bf_storage();
void tester_function_call_bf_storage(BFStorage *bfStorage);

#endif //BFSTORAGE_H
