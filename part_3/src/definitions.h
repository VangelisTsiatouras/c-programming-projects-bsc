#ifndef DEFS_H
#define DEFS_H

//////////////////////////////////////
//      PARAMETER DEFINES

// Bloom Filter
#define K 3                 // Number of hash functions
#define PROBABILITY 0.000001  // Probability of false positives
#define STARTING_M 40099    // Starting bit vector size
#define SEED1 12345678
#define SEED2 87654321
#define SEED3 54321876

// Linear Hash
#define LH_BUCKET_SIZE 8
#define LH_STARTING_SIZE 32

// Ngram Counter
#define NC_STATIC_HASH_SIZE 4999
#define NC_BUCKET_SIZE 4
#define MURMUR_SEED 5

// Query Results
#define DEFAULT_LINES 8
#define DEFAULT_LINE_SIZE 64
#define DEFAULT_PRINT_BUFFER 1024

// Trie
#define STARTING_SIZE_CHILD_ARRAY 4
#define WORD_SIZE 20
#define DEFAULT_NGRAM_WORDS 10
#define DEFAULT_QUERY_BUFFER 20

// Threads
#define NUMBER_OF_THREADS 50


//////////////////////////////////////
//        ERROR DEFINES

#define MALLOC_ERROR -1
#define REALLOC_ERROR -2
#define FOPEN_ERROR -3
#define NOT_ALLOCATED_ERROR -4
#define DELETE_NOT_FOUND 2
#define ADD_LINE_FOUND 1
#define BLOOM_FILTER_FOUND 3
#define WRONG_F_VALUE 4
#define SUCCESS 0

//////////////////////////////////////

#endif //DEFS_H
