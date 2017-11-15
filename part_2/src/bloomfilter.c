#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "bloomfilter.h"
#include "defs.h"

BloomFilter *bloom_filter_create() {
    BloomFilter *bloomFilter = malloc(sizeof(BloomFilter));
    if (!bloomFilter) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    bloomFilter->bitVectorSize = M;
    bloomFilter->numberOfHashFunctions = K;
    bloomFilter->bitVector = malloc(sizeof(short) * bloomFilter->bitVectorSize);
    if (!bloomFilter->bitVector) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    // Set bit vector to 0
    bloom_filter_set_to_zero(bloomFilter);

    // Calculate false positive probability p
    bloomFilter->expectedProbFalsePositives = pow(1.0 - pow(1.0 - (1.0 / (double) M), (double) K * (double) N),
                                                  (double) K);
    printf("Bit vector size: %d\nNumber of hash functions: %d\nExpected false positive probability for %d elements: %lf\n",
           M, K, N, bloomFilter->expectedProbFalsePositives);

    return bloomFilter;
}

void bloom_filter_destroy(BloomFilter *bloomFilter) {
    free(bloomFilter->bitVector);
    free(bloomFilter);
}

void bloom_filter_set_to_zero(BloomFilter *bloomFilter) {
    for (int i = 0; i < bloomFilter->bitVectorSize; i++) {
        bloomFilter->bitVector[i] = 0;
    }
}

int bloom_filter_check_insert(BloomFilter *bloomFilter, char *ngram) {

    int position, notFound = 0;
    uint32_t seed1 = 12345678, seed2 = 87654321;
    uint32_t hashes[3];
    hashes[0] = murmurHash3(ngram, (uint32_t) strlen(ngram), seed1);
    hashes[1] = murmurHash3(ngram, (uint32_t) strlen(ngram), seed2);
    hashes[2] = hashes[0] + 2 * hashes[1]; // Kirsch-Mitzenmacher-Optimization

    for (int i = 0; i < 3; i++) {
        position = hashes[i] % bloomFilter->bitVectorSize;
        //        printf("    murmur3(seed %d)=%u, position=%d\n", i, hash, position);
        if (bloomFilter->bitVector[position] == 0) {
            // Change bitVector[position] to 1
            bloomFilter->bitVector[position] = 1;
            notFound = 1;
        }
    }
    if (notFound == 1) {
        //call add_line
        return SUCCESS;
    } else {
        return BLOOM_FILTER_FOUND;
    }
}

void print_bit_vector(BloomFilter *bloomFilter) {
    for(int i = 0; i < bloomFilter->bitVectorSize; i++) {
        printf("%d ", bloomFilter->bitVector[i]);
    }
    printf("\n");
}