#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
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
    for (int i = 0; i < lhBucket->capacity; i++) {
        destroy_trie_node(&(lhBucket->nodeArray[i]));
    }
    free(lhBucket->nodeArray);
    free(lhBucket);
}

int insert_LHBucket(LHBucket *lhBucket, char* word) {
    //TrieNode *current = lhBucket->nodeArray;
    SearchResults result;
    // Don't call binary_search if the children array is empty
    if (lhBucket->occupiedPositions == 0) {
        result.position = 0;
        result.found = 0;
    } else {
        result = binary_search(lhBucket->nodeArray, word, lhBucket->occupiedPositions);
    }
    int position = result.position;
    if (result.found == 0) {
        ///expand_if_full_LHBucket(lhBucket);
//        // Reallocate space if the children array is full
//        if (current->occupiedPositions == current->capacity) {
//            // The new size will be the double of the old size
//            current->capacity *= 2;
//            TrieNode *tempChildren = realloc(current->children, current->capacity * sizeof(TrieNode));
//            if (tempChildren == NULL) {
//                printf("realloc error %s\n", strerror(errno));
//                exit(REALLOC_ERROR);
//            }
//            current->children = tempChildren;
//        }
        // Shift elements to keep the children array sorted
        if (position < lhBucket->occupiedPositions) {
            memmove(&lhBucket->nodeArray[position + 1], &lhBucket->nodeArray[position], sizeof(TrieNode) * (lhBucket->occupiedPositions - position));
        }

//        if (position < lhBucket->occupiedPositions) {
//            memmove(&lhBucket->nodeArray[position + 1], &lhBucket->nodeArray[position],
//                    sizeof(TrieNode) * (lhBucket->occupiedPositions - position));
//        }
        create_trie_node(&lhBucket->nodeArray[position]);
        store_word_trie_node(&lhBucket->nodeArray[position], word);
        lhBucket->occupiedPositions++;
        return SUCCESS;
    }
    return 1;
}

void delete_LHBucket(LHBucket *lhBucket, char* word) {

}

void expand_if_full_LHBucket(LHBucket *lhBucket) {//todo call creat....
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
    }
}

void print_LHBucket(LHBucket *lhBucket) {
    for (int i = 0; i < lhBucket->occupiedPositions; i++) {
        printf("%s ", get_word_trie_node(&lhBucket->nodeArray[i]));
    }
    printf("\n");
}

///

LinearHash *create_LinearHash() {
    LinearHash *linearHash = malloc(sizeof(LinearHash));
    if (!linearHash) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    linearHash->bucketArray = malloc(LH_STARTING_SIZE * sizeof(LHBucket*));
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
        destroy_LHBucket(linearHash->bucketArray[i]);
    }
    free(linearHash->bucketArray);
    free(linearHash);
}

void linearHashTester() {
    //LinearHash *linearHash = create_LinearHash();
    LHBucket *lhBucket = create_LHBucket();
    printf("%d\n", sizeof(LHBucket));
    printf("%d\n", insert_LHBucket(lhBucket, "d"));
    printf("%d\n", insert_LHBucket(lhBucket, "c"));
    printf("%d\n", insert_LHBucket(lhBucket, "a"));
    printf("%d\n", insert_LHBucket(lhBucket, "f"));
//    printf("%d\n", insert_LHBucket(linearHash->bucketArray[0], "a"));
//    printf("%d\n", insert_LHBucket(linearHash->bucketArray[0], "f"));
    print_LHBucket(lhBucket);
    destroy_LHBucket(lhBucket);

    //destroy_LinearHash(linearHash);
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