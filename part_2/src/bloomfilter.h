#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "hashfunctions.h"

#define K 3                 // Number of hash functions
#define STARTING_M 40099    // Starting bit vector size
#define SEED1 12345678
#define SEED2 87654321

typedef struct BloomFilter {
    char *bitVector;
    size_t bitVectorSize;
    double acceptedProbability;
} BloomFilter;

BloomFilter *create_bloom_filter();
void destroy_bloom_filter(BloomFilter *bloomFilter);
void resize_bit_vector(BloomFilter *bloomFilter, size_t newSize);
void set_to_zero_bloom_filter(BloomFilter *bloomFilter);
void probability_of_query_bloom_filter(BloomFilter *bloomFilter, int numberOfWords);
int check_insert_bloom_filter(BloomFilter *bloomFilter, char *ngram);
void print_bit_vector(BloomFilter *bloomFilter);
int compare_double(double a, double b);

#endif //BLOOMFILTER_H
