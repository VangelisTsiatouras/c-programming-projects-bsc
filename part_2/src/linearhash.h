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

typedef struct LookupStruct {
    TrieNode *trieNode;
    unsigned int bucket;
} LookupStruct;

LHBucket *create_LHBucket();
void destroy_LHBucket(LHBucket *lhBucket);
int insert_word_LHBucket(LHBucket *lhBucket, char *word);
int insert_trie_node_LHBucket(LHBucket *lhBucket, TrieNode *trieNode);
void delete_word_LHBucket(LHBucket *lhBucket, char* word);
int expand_if_full_LHBucket(LHBucket *lhBucket);
int print_LHBucket(LHBucket *lhBucket);

LinearHash *create_LinearHash();
void destroy_LinearHash(LinearHash *linearHash);
TrieNode *insert_LinearHash(LinearHash *linearHash, char *word);
TrieNode *lookup_LinearHash(LinearHash *linearHash, char *word);
LookupStruct lookup_for_delete_LinearHash(LinearHash *linearHash, char *word);
int expand_LinearHash(LinearHash *linearHash);
int rehash_bucket_LinearHash(LinearHash *linearHash, int bucket);

void print_LinearHash(LinearHash *linearHash);
void print_node_children_LinearHash(LinearHash *linearHash);

unsigned int old_h(LinearHash *linearHash, char *ngram, size_t length);
unsigned int new_h(LinearHash *linearHash, char *ngram, size_t length);

#endif //LINEARHASH_H
