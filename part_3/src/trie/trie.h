#ifndef TRIE_H
#define TRIE_H

#include "../definitions.h"
#include "../bloomfilter/bloomfilter.h"
#include "../queryresults/queryresults.h"
#include "../ngramcounter/ngramcounter.h"
#include "../bloomfilterstorage/bfstorage.h"

typedef struct LinearHash LinearHash;

typedef struct TrieNode {
    char word[WORD_SIZE];
    char *largeWord;
    char isFinal;
    char isDeleted; // TODO isws auto to pedio na mhn xreiazetai telika
    int appendVersion;
    int deleteVersion;
    int markedAsFinalVersion;
    struct TrieNode *children;
    int occupiedPositions;
    int capacity;
    short *staticTrieWordOffsets;
    unsigned int staticArraySize;
} TrieNode;

typedef struct Trie {
    LinearHash *linearHash;
} Trie;

// This structure is used from binary_search
typedef struct SearchResults {
    char found;
    int position;
} SearchResults;

Trie *create_trie();
int destroy_trie(Trie *trie);
int insert_ngram_trie(Trie *trie, char *ngram);
int insert_ngram_version_trie(Trie *trie, char *ngram, int version);
void query_trie_dynamic(Trie *trie, char *ngram, BloomFilter *bloomFilter, QueryResults *queryResults,
                        NgramCounter *ngramCounter, int queryID, int totalQueries, int version);
void query_trie_static(Trie *trie, char *ngram, BFStorage *bloomFilterStorage, QueryResults *queryResults,
                       NgramCounter *ngramCounter, int *queryID, int *totalQueries);
void compress_trie(Trie *trie);
int delete_ngram_trie(Trie *trie, char *ngram);
int delete_ngram_version_trie(Trie *trie, char *ngram, int version);

int create_trie_node(TrieNode *trieNode);
void destroy_trie_node(TrieNode *trieNode);
void store_word_trie_node(TrieNode *trieNode, char *word);
void compress_trie_node(TrieNode *trieNode);
char* get_word_trie_node(TrieNode *trieNode);
int is_empty_trie_node(TrieNode *trieNode);
void delete_word_trie_node(TrieNode *trieNode, int position);
void print_trie_node(TrieNode *trieNode);

SearchResults binary_search(TrieNode *childrenArray, char *word, int occupiedPositions);
char **split_ngram(char *ngram, int *numberOfWords);
void realloc_buffer(char **buffer, size_t *sizeBuffer , size_t newSize);
int compare_n_chars(char *buffer1, char *buffer2, int n);
void trie_dfs_print(TrieNode *trieNode);

void tester_compress();

#endif //TRIE_H
