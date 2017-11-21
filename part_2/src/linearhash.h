#ifndef LINEARHASH_H
#define LINEARHASH_H

#include "trie.h"

#define LH_BUCKET_SIZE 4
#define LH_STARTING_SIZE 4

typedef struct LHBucket {
    TrieNode *nodeArray;
    unsigned int capacity;
    unsigned int occupiedPositions;
} LHBucket;

typedef struct LinearHash {
    LHBucket **bucketArray;
    unsigned int arraySize;
    unsigned int round;
    unsigned int p;
} LinearHash;

LHBucket *create_LHBucket();
void destroy_LHBucket(LHBucket *lhBucket);
int insert_LHBucket(LHBucket *lhBucket, char* word);
void delete_LHBucket(LHBucket *lhBucket, char* word);
void expand_if_full_LHBucket(LHBucket *lhBucket);
void print_LHBucket(LHBucket *lhBucket);

LinearHash *create_LinearHash();
void destroy_LinearHash(LinearHash *linearHash);

void linearHashTester();

#endif //LINEARHASH_H
