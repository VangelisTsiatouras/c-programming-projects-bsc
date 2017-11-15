#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "defs.h"
#include "ngramcounter.h"
#include "hashfunctions.h"

void allocate_ncbucket_array(NCBucket *ncBucket) {
    ncBucket->arraySize = NC_BUCKET_SIZE;
    ncBucket->array = malloc(NC_BUCKET_SIZE * sizeof(Pair));
    if (!ncBucket->array) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
};

void expand_ncbucket_array(NCBucket *ncBucket) {
    //printf("expanding from %d to %d\n", ncBucket->arraySize, ncBucket->arraySize*2); //todo remove print
    ncBucket->arraySize *= 2;
    ncBucket->array = realloc(ncBucket->array, ncBucket->arraySize * sizeof(Pair));
    if (!ncBucket->array) {
        printf("realloc error %s\n", strerror(errno));
        exit(REALLOC_ERROR);
    }
};

void destroy_ncbucket_array(NCBucket *ncBucket) {
    for (int i = 0; i < ncBucket->arraySize; i++) {
        if (ncBucket->array[i].ngram == NULL) {
            break;
        } else {
            free(ncBucket->array[i].ngram);
        }
    }
    free(ncBucket->array);
};

int insertNCBucketArray(NCBucket *ncBucket, char *ngram, unsigned int length) {
    int i;
    for (i = 0; i < ncBucket->arraySize; i++) {
        if (ncBucket->array[i].ngram == NULL) {
            break;
        }
        if (strcmp(ngram, ncBucket->array[i].ngram) == 0) {
            ncBucket->array[i].counter++;
            return 0;
        }
    }
    // If we are here, it means that it wasn't found
    if (i >= ncBucket->arraySize) {
        // If it doens't fit, expand the bucket
        expand_ncbucket_array(ncBucket);
    }
    // Allocate space for the ngram
    ncBucket->array[i].ngram = malloc(length * sizeof(char));
    if (!ncBucket->array[i].ngram) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    strcpy(ncBucket->array[i].ngram, ngram);
    ncBucket->array[i].counter = 1;
    return 1;
}

void clear_ncbucket_array(NCBucket *ncBucket) {
    for (int i = 0; i < ncBucket->arraySize; i++) {
        if (ncBucket->array[i].ngram != NULL) {
            free(ncBucket->array[i].ngram);
            ncBucket->array[i].ngram = NULL;
            ncBucket->array[i].counter = 0;
        }
        else {
            break;
        }
    }
}

void print_ncbucket_array(NCBucket *ncBucket) {
    for (int i = 0; i < ncBucket->arraySize; i++) {
        if (ncBucket->array[i].ngram == NULL) {
            break;
        }
        printf("|%s:%d", ncBucket->array[i].ngram, ncBucket->array[i].counter);
    }
    printf("\n");
}

///

NgramCounter *create_ngram_counter () {
    NgramCounter *ngramCounter = malloc(sizeof(NgramCounter));
    if (!ngramCounter) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        allocate_ncbucket_array(&(ngramCounter->buckets[i]));
    }
    return ngramCounter;
};

void destroy_gram_counter(NgramCounter *ngramCounter) {
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        destroy_ncbucket_array(&(ngramCounter->buckets[i]));
    }
    free(ngramCounter);
};

int insert_ngram_counter(NgramCounter *ngramCounter, char *ngram) {
    //todo optimize ama 3eroume to length apo prin, na mpei trito argument gia na apofugoume thn strlen
    unsigned int length = (unsigned int)strlen(ngram) + 1;
    int position = hash_function(ngram, length);
    return insertNCBucketArray(&(ngramCounter->buckets[position]), ngram, length);
};

int clear_ngram_counter(NgramCounter *ngramCounter) {
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        clear_ncbucket_array(&(ngramCounter->buckets[i]));
    }
};

unsigned int hash_function(char *ngram, unsigned int length) {
    return murmurHash3(ngram, length, MURMUR_SEED) % NC_STATIC_HASH_SIZE;
};

void print_ngram_counter(NgramCounter *ngramCounter) {
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        printf("Bucket[%d]", i);
        print_ncbucket_array(&(ngramCounter->buckets[i]));
    }
}