#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "hashfunctions.h"

#define K 3        // Number of hash functions
#define M 20000    // Bit vector size
#define N 1000      // Expected number of elements to be filtered

typedef struct BloomFilter {
    short *bitVector;
    int bitVectorSize;
    int numberOfHashFunctions;
    double expectedProbFalsePositives;
} BloomFilter;

BloomFilter *bloom_filter_create();

void bloom_filter_destroy(BloomFilter *bloomFilter);

void bloom_filter_set_to_zero(BloomFilter *bloomFilter);

int bloom_filter_check_insert(BloomFilter *bloomFilter,char *ngram);

#endif //BLOOMFILTER_H
