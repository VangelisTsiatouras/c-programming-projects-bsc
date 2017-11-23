#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "linearhash.h"

LHBucket *create_LHBucket() {
    LHBucket *lhBucket = malloc(sizeof(LHBucket));
    if (!lhBucket) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    lhBucket->nodeArray = malloc(LH_BUCKET_SIZE * sizeof(TrieNode));
    if (!lhBucket->nodeArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    lhBucket->capacity = LH_BUCKET_SIZE;
    lhBucket->occupiedPositions = 0;
//    for (int i = 0; i < LH_BUCKET_SIZE; i++) {
//        create_trie_node(&lhBucket->nodeArray[i]);
//    }
    return lhBucket;
}

void destroy_LHBucket(LHBucket *lhBucket) {
    for (int i = 0; i < lhBucket->occupiedPositions; i++) {
        destroy_trie_node(&(lhBucket->nodeArray[i]));
    }
    free(lhBucket->nodeArray);
    free(lhBucket);
}

int insert_word_LHBucket(LHBucket *lhBucket, char *word) {
    //TrieNode *current = lhBucket->nodeArray;
    int returnValue;
    SearchResults result;
    // Don't call binary_search if the bucket is empty
    if (lhBucket->occupiedPositions == 0) {
        result.position = 0;
        result.found = 0;
    } else {
        result = binary_search(lhBucket->nodeArray, word, lhBucket->occupiedPositions);
    }
    int position = result.position;
    if (result.found == 0) {
        returnValue = expand_if_full_LHBucket(lhBucket);
        // Shift elements to keep the node array sorted
        if (position < lhBucket->occupiedPositions) {
            memmove(&lhBucket->nodeArray[position + 1], &lhBucket->nodeArray[position],
                    sizeof(TrieNode) * (lhBucket->occupiedPositions - position));
        }
        create_trie_node(&lhBucket->nodeArray[position]);
        store_word_trie_node(&lhBucket->nodeArray[position], word);
        lhBucket->occupiedPositions++;
        return returnValue;
    }
    return 0;
}

int insert_trie_node_LHBucket(LHBucket *lhBucket, TrieNode *trieNode) {
    //TrieNode *current = lhBucket->nodeArray;
    int returnValue;
    SearchResults result;
    // Don't call binary_search if the node array is empty
    if (lhBucket->occupiedPositions == 0) {
        result.position = 0;
        result.found = 0;
    } else {
        result = binary_search(lhBucket->nodeArray, get_word_trie_node(trieNode), lhBucket->occupiedPositions);
    }
    int position = result.position;
    if (result.found == 0) {
        returnValue = expand_if_full_LHBucket(lhBucket);
        // Shift elements to keep the children array sorted
        if (position < lhBucket->occupiedPositions) {
            memmove(&lhBucket->nodeArray[position + 1], &lhBucket->nodeArray[position],
                    sizeof(TrieNode) * (lhBucket->occupiedPositions - position));
        }
        memmove(&lhBucket->nodeArray[position], trieNode, sizeof(TrieNode));
        lhBucket->occupiedPositions++;
        float load = lhBucket->occupiedPositions / (float) lhBucket->capacity; //todo na ginei swsta
        if (load > LH_SPLIT_CONDITION) {
            returnValue = 1;
        }
        return returnValue;
    }
    return 0;
}

void delete_LHBucket(LHBucket *lhBucket, char *word) {

}

int expand_if_full_LHBucket(LHBucket *lhBucket) {
    // Reallocate space if the children array is full
    if (lhBucket->occupiedPositions == lhBucket->capacity) {
        // The new size will be the double of the old size
        lhBucket->capacity *= 2;
        TrieNode *tempArray = realloc(lhBucket->nodeArray, lhBucket->capacity * sizeof(TrieNode));
        if (tempArray == NULL) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
        lhBucket->nodeArray = tempArray;
        return 1;
    }
    return 0;
}

int print_LHBucket(LHBucket *lhBucket) {
    for (int i = 0; i < lhBucket->occupiedPositions; i++) {
        printf("%s ", get_word_trie_node(&lhBucket->nodeArray[i]));
    }
    printf("\n");
    return lhBucket->occupiedPositions;
}

///

LinearHash *create_LinearHash() {
    LinearHash *linearHash = malloc(sizeof(LinearHash));
    if (!linearHash) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    linearHash->bucketArray = malloc(LH_STARTING_SIZE * sizeof(LHBucket *));
    if (!linearHash->bucketArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    linearHash->arraySize = LH_STARTING_SIZE;
    linearHash->p = 0;
    linearHash->round = 0;
    for (int i = 0; i < LH_STARTING_SIZE; i++) {
        linearHash->bucketArray[i] = create_LHBucket();
    }
    return linearHash;
}

void destroy_LinearHash(LinearHash *linearHash) {
    for (int i = 0; i < linearHash->arraySize; i++) {
        if (linearHash->bucketArray[i] != NULL) {
            destroy_LHBucket(linearHash->bucketArray[i]);
        }
    }
    free(linearHash->bucketArray);
    free(linearHash);
}

TrieNode *insert_LinearHash(LinearHash *linearHash, char *word) {
    size_t length = strlen(word);
    int hash = old_h(linearHash, word, length);
    if (hash < linearHash->p) {
        hash = new_h(linearHash, word, length); // Hash using next round's function
    }
    // If we need to reallocate
    if (hash > linearHash->arraySize) {
        expand_LinearHash(linearHash);
    }
    if (linearHash->bucketArray[hash] == NULL) {
        linearHash->bucketArray[hash] = create_LHBucket();
    }
    int r = insert_word_LHBucket(linearHash->bucketArray[hash], word); // returns 1 if there was an overflow
    // If we need to split a bucket
    if (r == 1) {
//        printf("overflow!\n");
        int hashAgain;
        do {
            if (linearHash->bucketArray[linearHash->p] == NULL) {
                linearHash->p += 1;
            }
            hashAgain = rehash_bucket_LinearHash(linearHash, linearHash->p);
            linearHash->p += 1;
            if (linearHash->p == linearHash->arraySize - 1) {
                linearHash->p = 0;
                linearHash->round++;
            }
        } while(hashAgain == 1);


    }
    return NULL;
}

TrieNode *lookup_LinearHash(LinearHash *linearHash, char *word) {
    size_t length = strlen(word);
    int hash = old_h(linearHash, word, length);
    if (hash < linearHash->p) {
        hash = new_h(linearHash, word, length); // Hash using next round's function
    }
    LHBucket *bucket = linearHash->bucketArray[hash];
    SearchResults result;
    // Don't call binary_search if the bucket is empty
    if (bucket == NULL || bucket->occupiedPositions == 0) {
        return NULL;
    } else {
        result = binary_search(bucket->nodeArray, word, bucket->occupiedPositions);
    }
    if(result.found == 1) {
        return (&bucket->nodeArray[result.position]);
    }
    return NULL;
}

int expand_LinearHash(LinearHash *linearHash) {
    linearHash->arraySize *= 2;
    linearHash->bucketArray = realloc(linearHash->bucketArray, linearHash->arraySize * sizeof(LHBucket));//todo
    if (linearHash->bucketArray == NULL) {
        printf("realloc error %s\n", strerror(errno));
        exit(REALLOC_ERROR);
    }
    for (int i = linearHash->arraySize / 2; i < linearHash->arraySize; i++) {
        linearHash->bucketArray[i] = NULL;
    }
}

int rehash_bucket_LinearHash(LinearHash *linearHash, int bucketPos) {
    int againOverflow = 0;
    unsigned int firstEmptyPosition = 0;
    LHBucket *current = linearHash->bucketArray[bucketPos];
    int hash;
    for (int i = 0; i < current->occupiedPositions; i++) {
        char *word = get_word_trie_node(&current->nodeArray[i]);
        hash = new_h(linearHash, word, strlen(word)); // Hash using next round's function
        if (hash == bucketPos) {
            if (firstEmptyPosition < i) {
                memmove(&current->nodeArray[firstEmptyPosition], &current->nodeArray[i], sizeof(TrieNode));
            }
            firstEmptyPosition++;
        } else {
            if (hash >= linearHash->arraySize) {
                expand_LinearHash(linearHash);
            }
            if (linearHash->bucketArray[hash] == NULL) {
                linearHash->bucketArray[hash] = create_LHBucket();
            }
//            for (int j = 0; j < current->occupiedPositions; j++) {
//                printf("%p\n", current->nodeArray[i]);
//            }
            if (insert_trie_node_LHBucket(linearHash->bucketArray[hash], &current->nodeArray[i])) {
                againOverflow = 1;
            }
//            current->occupiedPositions--;
        }
        //current->occupiedPositions = firstEmptyPosition;
    }
    current->occupiedPositions = firstEmptyPosition;
    return againOverflow;
}

void print_LinearHash(LinearHash *linearHash) {
    int s = 0;
    int max = -1;
    for (int i = 0; i < linearHash->arraySize; i++) {
        if (linearHash->p == i) {
            printf("p->[%d]", i);
        } else {
            printf("   [%d]", i);
        }
        if (linearHash->bucketArray[i] == NULL) {
            printf("NULL\n");
            continue;
        }
        int temp = print_LHBucket(linearHash->bucketArray[i]);
        s += temp;
        if (max < temp) {
            max = temp;
        }
    }
    printf("Total elements %d\n", s);
    printf("Max bucket %d\n", max);
    printf("Average: %.1f\n", (float)s / linearHash->arraySize);
}

unsigned int old_h(LinearHash *linearHash, char *ngram, size_t length) {
    int mod = ((int) pow(2.0, (double) (linearHash->round)) *
               LH_STARTING_SIZE); // kmod(2^i)*m, where i = current round and m = LH_STARTING_SIZE
    return murmurHash3(ngram, (unsigned int) length, MURMUR_SEED) % mod;
}

unsigned int new_h(LinearHash *linearHash, char *ngram, size_t length) {
    int mod = ((int) pow(2.0, (double) (linearHash->round + 1)) *
               LH_STARTING_SIZE); // kmod(2^(i+1))*m, where i = current round and m = LH_STARTING_SIZE
    return murmurHash3(ngram, (unsigned int) length, MURMUR_SEED) % mod;
}

void linearHashTester() {
    //srand(time(NULL));
    LinearHash *linearHash = create_LinearHash();
    char string[20] = "aaaa";
    for (int i = 0; i < 500000; i++) {
        char c = rand() % 26 + 'a';
        string[0] = c;
        c = rand() % 26 + 'a';
        string[1] = c;
        c = rand() % 26 + 'a';
        string[2] = c;
        c = rand() % 26 + 'a';
        string[3] = c;
        insert_LinearHash(linearHash, string);
    }
    print_LinearHash(linearHash);
    TrieNode *tn = lookup_LinearHash(linearHash, "bmcf");
    if (tn != NULL){
        printf("%s\n", tn->word);
    }
    tn = lookup_LinearHash(linearHash, "xxxxxxx");
    if (tn != NULL){
        printf("%s\n", tn->word);
    }
//    insert_LinearHash(linearHash, "vaggelis");
//    insert_LinearHash(linearHash, "vaggelis1");
//    insert_LinearHash(linearHash, "vaggelis2");



    destroy_LinearHash(linearHash);
    exit(0);
}

void bucketTester() {
    LHBucket *lhBucket = create_LHBucket();
    printf("%ld\n", sizeof(LHBucket));
    printf("%d\n", insert_word_LHBucket(lhBucket, "d"));
    printf("%d\n", insert_word_LHBucket(lhBucket, "c"));
    printf("%d\n", insert_word_LHBucket(lhBucket, "a"));
    printf("%d\n", insert_word_LHBucket(lhBucket, "f"));
    printf("%d\n", insert_word_LHBucket(lhBucket, "b"));
    TrieNode t;
    create_trie_node(&t);
    store_word_trie_node(&t, "vaggelis");
    insert_trie_node_LHBucket(lhBucket, &t);
    print_LHBucket(lhBucket);
    destroy_LHBucket(lhBucket);
    exit(0);
}







//    int position = -1; // Will point at first empty position
//    for (int i = 0; i < lhBucket->capacity; i++) {
//        if (isEmpty(&lhBucket->nodeArray[i])) {
//            if (position == -1) {
//                position = i;
//            }
//        }
//        else {
//            if (strcmp(word, get_word_trie_node(&lhBucket->nodeArray[i])) == 0){
//                return 1;//todo
//            }
//        }
//    }
//    store_word_trie_node(&lhBucket->nodeArray[position], word);
//    return SUCCESS;