#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "bloomfilter.h"

BloomFilter *create_bloom_filter() {
    BloomFilter *bloomFilter = malloc(sizeof(BloomFilter));
    if (!bloomFilter) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    bloomFilter->bitVectorSize = STARTING_M;
    bloomFilter->acceptedProbability = PROBABILITY;
    bloomFilter->bitVector = malloc(sizeof(short) * bloomFilter->bitVectorSize);
    if (!bloomFilter->bitVector) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    return bloomFilter;
}

void destroy_bloom_filter(BloomFilter *bloomFilter) {
    free(bloomFilter->bitVector);
    free(bloomFilter);
}

void resize_bit_vector(BloomFilter *bloomFilter, size_t newSize) {
    bloomFilter->bitVectorSize = newSize;
    bloomFilter->bitVector = realloc(bloomFilter->bitVector, bloomFilter->bitVectorSize * sizeof(char));
    if (!bloomFilter->bitVector) {
        printf("realloc error %s\n", strerror(errno));
        exit(REALLOC_ERROR);
    }
}

void set_to_zero_bloom_filter(BloomFilter *bloomFilter) {
    memset(bloomFilter->bitVector, 0, bloomFilter->bitVectorSize);
}

void probability_of_query_bloom_filter(BloomFilter *bloomFilter, int numberOfWords) {
    double percentage = 70.0 / 100.0;
    int partOfWords = (int) (percentage * numberOfWords);

    // Expected probability calculation.
    double prob = pow(1.0 - exp(-((((double) bloomFilter->bitVectorSize / (double) partOfWords) * log(2))) *
                                ((double) partOfWords / (double) bloomFilter->bitVectorSize)),
                      (((double) bloomFilter->bitVectorSize / (double) partOfWords) * log(2)));

    // If the query's expected probality is bigger than 0.0001
    // Increase the size of the bit vector to a new size
    if (compare_double(prob, bloomFilter->acceptedProbability) > 0) {
        size_t newSize = (size_t) (-(partOfWords * log(bloomFilter->acceptedProbability) / pow(log(2), 2)));
        resize_bit_vector(bloomFilter, newSize);
    }
}

int check_insert_bloom_filter(BloomFilter *bloomFilter, char *ngram) {
    int position, notFound = 0;
    // Use 5 hash functions
    unsigned int hashes[5];
    size_t length = strlen(ngram);
    hashes[0] = murmurHash3(ngram, (unsigned int) length, SEED1);
    hashes[1] = murmurHash3(ngram, (unsigned int) length, SEED2);
    hashes[2] = murmurHash3(ngram, (unsigned int) length, SEED3);
    hashes[3] = murmurHash3(ngram, (unsigned int) length, SEED4);
    // Kirsch-Mitzenmacher-Optimization
    hashes[4] = hashes[0] + 2 * hashes[1];
    // For each hash function evaluate the position in bit vector
    for (int i = 0; i < 5; i++) {
        position = hashes[i] % (int) bloomFilter->bitVectorSize;
        if (bloomFilter->bitVector[position] == 0) {
            // Change bitVector[position] to 1
            bloomFilter->bitVector[position] = 1;
            notFound = 1;
        }
    }
    if (notFound == 1) {
        return SUCCESS;
    } else {
        return BLOOM_FILTER_FOUND;
    }
}

void print_bit_vector(BloomFilter *bloomFilter) {
    for (int i = 0; i < bloomFilter->bitVectorSize; i++) {
        printf("%d ", bloomFilter->bitVector[i]);
    }
    printf("\n");
}

int compare_double(double a, double b) {
    return fabs(a) > fabs(b);
}
