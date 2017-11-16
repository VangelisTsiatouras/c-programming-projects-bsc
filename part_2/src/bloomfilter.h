#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "hashfunctions.h"

#define K 3        // Number of hash functions
#define M 40099    // Bit vector size
#define N 1000     // Expected number of elements to be filtered

typedef struct BloomFilter {
    char *bitVector;
    size_t bitVectorSize;
    int numberOfHashFunctions;
    double expectedProbFalsePositives;
} BloomFilter;

BloomFilter *create_bloom_filter();
void destroy_bloom_filter(BloomFilter *bloomFilter);
void set_to_zero_bloom_filter(BloomFilter *bloomFilter);
int check_insert_bloom_filter(BloomFilter *bloomFilter, char *ngram);
void print_bit_vector(BloomFilter *bloomFilter);

#endif //BLOOMFILTER_H
