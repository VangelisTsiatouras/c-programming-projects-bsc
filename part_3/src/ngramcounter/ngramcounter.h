#ifndef NGRAMCOUNTER_H
#define NGRAMCOUNTER_H

#include "../hashfunctions/hashfunctions.h"
#include "../definitions.h"

typedef struct Pair {
    char* ngram;
    unsigned int counter;
} Pair;

typedef struct NCBucket {
    Pair *array;
    unsigned int arraySize;
} NCBucket;

typedef struct NgramCounter {
    NCBucket buckets[NC_STATIC_HASH_SIZE];
    unsigned int elements;
} NgramCounter;

typedef NCBucket NgramArray;

void allocate_ncbucket_array(NCBucket *ncBucket);
void expand_ncbucket_array(NCBucket *ncBucket);
void destroy_ncbucket_array(NCBucket *ncBucket);
int insert_ncbucket_array(NCBucket *ncBucket, char *ngram, unsigned int length);
void clear_ncbucket_array(NCBucket *ncBucket);
void print_ncbucket_array(NCBucket *ncBucket);

NgramCounter *create_ngram_counter();
void destroy_gram_counter(NgramCounter *ngramCounter);
int insert_ngram_counter(NgramCounter *ngramCounter, char *ngram, unsigned int ngramLength);
void clear_ngram_counter(NgramCounter *ngramCounter);
unsigned int hash_function(char *ngram, unsigned int length);
void print_ngram_counter(NgramCounter *ngramCounter);

NgramArray *copy_to_ngram_array(NgramCounter *ngramCounter);
void destroy_ngram_array(NgramArray* ngramArray);
void print_ngram_array(NgramArray* ngramArray);

void swap(Pair *a, Pair *b);
int pair_compare(const void *a, const void *b);
unsigned int partition(Pair *A, unsigned int left, unsigned int right);
unsigned int quick_select(Pair *A, unsigned int left, unsigned int right, int k);
void sort_topk(NgramArray *ngramArray, unsigned int k);

void tester_ngram_counter();



#endif //NGRAMCOUNTER_H
