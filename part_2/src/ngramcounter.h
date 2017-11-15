#ifndef NGRAMCOUNTER_H
#define NGRAMCOUNTER_H

#define NC_STATIC_HASH_SIZE 1009 // a big prime number todo choose optimal number
#define NC_BUCKET_SIZE 4
#define MURMUR_SEED 5

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
} NgramCounter;

void allocateNCBucketArray(NCBucket *ncBucket);
void expandNCBucketArray(NCBucket *ncBucket);
void destroyNCbucketArray(NCBucket *ncBucket);
int insertNCBucketArray(NCBucket *ncBucket, char *ngram, unsigned int length);
void clearNCBucketArray(NCBucket *ncBucket);
void printNCBucketArray(NCBucket *ncBucket);

NgramCounter *create_ngram_counter();
void destroy_gram_counter(NgramCounter *ngramCounter);
int insert_ngram_counter(NgramCounter *ngramCounter, char *ngram);
int clear_ngram_counter(NgramCounter *ngramCounter);
unsigned int hashFunction(char *ngram, unsigned int length);
void print_ngram_counter(NgramCounter *ngramCounter);




#endif //NGRAMCOUNTER_H
