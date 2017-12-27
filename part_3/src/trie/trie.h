#ifndef TRIE_H
#define TRIE_H

#include "../definitions.h"
#include "../bloomfilter/bloomfilter.h"
#include "../oldqueryresults/oldqueryresults.h"
#include "../ngramcounter/ngramcounter.h"

typedef struct LinearHash LinearHash;

typedef struct TrieNode {
    char word[WORD_SIZE];
    char *largeWord;
    struct TrieNode *children;
    int occupiedPositions;
    int capacity;
    char isFinal;
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
int insert_trie(Trie *trie, char *ngram);
void query_trie_dynamic(Trie *trie, char *ngram, BloomFilter *bloomFilter, QueryResults *queryResults,
                        NgramCounter *ngramCounter);
void query_trie_static(Trie *trie, char *ngram, BloomFilter *bloomFilter, QueryResults *queryResults,
                NgramCounter *ngramCounter);
void compress_trie(Trie *trie);
int delete_ngram_trie(Trie *trie, char *ngram);

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
