#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "../hashfunctions/hashfunctions.h"
#include "../definitions.h"

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
