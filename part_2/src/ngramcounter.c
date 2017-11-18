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
    for (int i = 0; i < NC_BUCKET_SIZE; i++) {
        ncBucket->array[i].ngram = NULL;
        ncBucket->array[i].counter = 0;
    }
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
    for (int i = ncBucket->arraySize / 2; i < ncBucket->arraySize; i++) {
        ncBucket->array[i].ngram = NULL;
        ncBucket->array[i].counter = 0;
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
        } else {
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

NgramCounter *create_ngram_counter() {
    NgramCounter *ngramCounter = malloc(sizeof(NgramCounter));
    if (!ngramCounter) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    ngramCounter->elements = 0;
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        allocate_ncbucket_array(&(ngramCounter->buckets[i]));
    }
    return ngramCounter;
}

void destroy_gram_counter(NgramCounter *ngramCounter) {
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        destroy_ncbucket_array(&(ngramCounter->buckets[i]));
    }
    free(ngramCounter);
}

int insert_ngram_counter(NgramCounter *ngramCounter, char *ngram) {
    //todo optimize ama 3eroume to length apo prin, na mpei trito argument gia na apofugoume thn strlen
    unsigned int length = (unsigned int) strlen(ngram) + 1;
    int position = hash_function(ngram, length);
    int returnValue = insertNCBucketArray(&(ngramCounter->buckets[position]), ngram, length);
    ngramCounter->elements += returnValue;
    return returnValue;
}

int clear_ngram_counter(NgramCounter *ngramCounter) {
    ngramCounter->elements = 0;
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        clear_ncbucket_array(&(ngramCounter->buckets[i]));
    }
}

unsigned int hash_function(char *ngram, unsigned int length) {
    return murmurHash3(ngram, length, MURMUR_SEED) % NC_STATIC_HASH_SIZE;
}

void print_ngram_counter(NgramCounter *ngramCounter) {
    printf("Elements: %d\n", ngramCounter->elements);
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        printf("Bucket[%d]", i);
        print_ncbucket_array(&(ngramCounter->buckets[i]));
    }
}

///

NgramArray *copy_to_ngram_array(NgramCounter *ngramCounter) {
    NgramArray *ngramArray = malloc(sizeof(NgramArray));
    if (!ngramArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    ngramArray->arraySize = ngramCounter->elements;
    ngramArray->array = malloc(ngramCounter->elements * sizeof(Pair));
    if (!ngramArray->array) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    int k = 0;
    for (int i = 0; i < NC_STATIC_HASH_SIZE; i++) {
        NCBucket *ncBucket = &(ngramCounter->buckets[i]);
        for (int j = 0; j < ncBucket->arraySize; j++) {
            if (ncBucket->array[j].ngram != NULL) {
                ngramArray->array[k] = ncBucket->array[j];
                k++;
                // To save time and memory, don't copy the strings from the static hash table,
                // just their pointers. Make sure you call clear_ngram_counter AFTER you're done with
                // the strings
            } else {
                break;
            }
        }
    }
    return ngramArray;
}

void destroy_ngram_array(NgramArray *ngramArray) {
    free(ngramArray->array);
    free(ngramArray);
}

void print_ngram_array(NgramArray *ngramArray) {
    printf("Elements: %d\n", ngramArray->arraySize);
    for (int i = 0; i < ngramArray->arraySize; i++) {
        printf("%s:%d\n", ngramArray->array[i].ngram, ngramArray->array[i].counter);
    }
}

///

void swap(Pair *a, Pair *b) {
    Pair temp = *a;
    *a = *b;
    *b = temp;
}

int pair_compare(const void *a, const void *b) {
    if (((Pair*)a)->counter > ((Pair*)b)->counter) {
        return -1;
    } else if (((Pair*)a)->counter < ((Pair*)b)->counter) {
        return 1;
    } else return strcmp(((Pair*)a)->ngram, ((Pair*)b)->ngram);
}

unsigned int partition(Pair *A, unsigned int left, unsigned int right) {
    Pair pivot = A[right];
    unsigned int i = left, x;
    for (x = left; x < right; x++) {
        if (pair_compare(&A[x], &pivot) < 0) { // A[x] >= pivot
            swap(&A[i], &A[x]);
            i++;
        }
    }
    swap(&A[i], &A[right]);
    return i;
}

unsigned int quick_select(Pair *A, unsigned int left, unsigned int right, int k) {
    //p is position of pivot in the partitioned array
    unsigned int p = partition(A, left, right);
    //k equals pivot got lucky
    if (p == k - 1) {
        return p;
    }
    else if (k - 1 < p) { //k less than pivot
        return quick_select(A, left, p - 1, k);
    }
    else { //k greater than pivot
        return quick_select(A, p + 1, right, k);
    }
}

void sort_topk(NgramArray *ngramArray, unsigned int k) {
    if (k > ngramArray->arraySize) {
        printf("Top k given (%d) is greater than the number of different ngrams\n", k);
        printf("Changing its value to %d\n", ngramArray->arraySize);
        k = ngramArray->arraySize;
    }
    // If the ngramArray is empty  just return
    if (ngramArray->arraySize == 0) {
        return;
    }
    quick_select(ngramArray->array, 0, ngramArray->arraySize - 1, k);
    qsort(ngramArray->array, k, sizeof(Pair), pair_compare);
    printf("Top: %s", ngramArray->array[0].ngram);
    for (int i = 1; i < k; i++) {
        printf("|%s", ngramArray->array[i].ngram);
    }
    printf("\n");
}

void tester() {
    NgramCounter* nc = create_ngram_counter();
    char str[3] = "aa";
    for (int i = 0; i < 10000; i++) {
        //printf("i: %d\n", i);
        int r = rand() % 26;
        int r2 = rand() % 26;
        str[0] += r;
        str[1] += r2;
        insert_ngram_counter(nc, str);
        str[0] -= r;
        str[1] -= r2;
    }
    NgramArray *na = copy_to_ngram_array(nc);
    print_ngram_array(na);
    sort_topk(na, nc->elements + 1);
    print_ngram_array(na);
    destroy_ngram_array(na);
    //print_ngram_counter(nc);
    destroy_gram_counter(nc);
//    insert_ngram_counter(nc, "a");
//    insert_ngram_counter(nc, "b");
//    insert_ngram_counter(nc, "c");
//    insert_ngram_counter(nc, "d");
//    insert_ngram_counter(nc, "e");
//    insert_ngram_counter(nc, "e");
//    insert_ngram_counter(nc, "e");
//    insert_ngram_counter(nc, "a");
//    print_ngram_counter(nc);
//    NgramArray *na = copy_to_ngram_array(nc, nc->elements);
//    print_ngram_array(na);
//    sort_topk(na, 4);
//    print_ngram_array(na);
//    destroy_ngram_array(na);
//    clear_ngram_counter(nc);
//    insert_ngram_counter(nc, "a");
//    na = copy_to_ngram_array(nc, nc->elements);
//    print_ngram_array(na);
//    destroy_ngram_array(na);
//    destroy_gram_counter(nc);
    exit(0);
}