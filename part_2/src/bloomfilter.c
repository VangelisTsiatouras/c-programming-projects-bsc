#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "bloomfilter.h"
#include "defs.h"

BloomFilter *create_bloom_filter() {
    BloomFilter *bloomFilter = malloc(sizeof(BloomFilter));
    if (!bloomFilter) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    bloomFilter->bitVectorSize = STARTING_M;
    bloomFilter->numberOfHashFunctions = K;
    bloomFilter->acceptedProbability = 0.0001;
    bloomFilter->bitVector = malloc(sizeof(short) * bloomFilter->bitVectorSize);
    if (!bloomFilter->bitVector) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }

    // Calculate starting false positive probability p
    bloomFilter->expectedProbFalsePositives = pow(1.0 - exp(-(double) K * (double) STARTING_N / (double) STARTING_M), (double) K);
    printf("Bit vector size: %d\nNumber of hash functions: %d\nExpected false positive probability for %d elements: %f%%\n",
           STARTING_M, K, STARTING_N, (double) 100.0 * bloomFilter->expectedProbFalsePositives);

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
    double percentage = 75.0 / 100.0;
    int partOfWords = (int) (percentage * numberOfWords);
    double prob = pow(1.0 - pow(1.0 - (1.0 / (double) bloomFilter->bitVectorSize), (double) K * (double) partOfWords),
                      (double) K);
    // If the query's expected probality is bigger than 0.0002
    // Increase the size of the bit vector to a new size
    if(compare_double(prob, bloomFilter->acceptedProbability) > 0) {
        size_t newSize = (size_t)(-(partOfWords * log(bloomFilter->acceptedProbability)/pow(log(2), 2)));
        resize_bit_vector(bloomFilter, newSize);
    }
}

int check_insert_bloom_filter(BloomFilter *bloomFilter, char *ngram) {
    int position, notFound = 0;
    uint32_t seed1 = 12345678, seed2 = 87654321;
    uint32_t hashes[3];
    hashes[0] = murmurHash3(ngram, (uint32_t) strlen(ngram), seed1);
    hashes[1] = murmurHash3(ngram, (uint32_t) strlen(ngram), seed2);
    // Kirsch-Mitzenmacher-Optimization
    hashes[2] = hashes[0] + 2 * hashes[1];
    for (int i = 0; i < 3; i++) {
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
    return fabs(a - b) > 0.01;
}