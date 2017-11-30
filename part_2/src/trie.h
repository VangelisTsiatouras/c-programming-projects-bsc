#ifndef TRIE_H
#define TRIE_H

#include "defs.h"
#include "bloomfilter.h"
#include "queryresults.h"
#include "ngramcounter.h"

#define STARTING_SIZE_CHILD_ARRAY 4
#define WORD_SIZE 20
#define DEFAULT_NGRAM_WORDS 10
#define DEFAULT_QUERY_BUFFER 20

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
int is_empty(TrieNode *trieNode);
void delete_word_trie_node(TrieNode *trieNode, int position);
void print_trie_node(TrieNode *trieNode);

SearchResults binary_search(TrieNode *childrenArray, char *word, int occupiedPositions);
char **split_ngram(char *ngram, int *numberOfWords);
void expand_results_buffer(char *buffer); ///
void trie_dfs_print(TrieNode *trieNode);

void tester_compress();

#endif //TRIE_H
