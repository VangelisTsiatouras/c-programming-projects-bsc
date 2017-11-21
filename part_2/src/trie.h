#ifndef TRIE_H
#define TRIE_H

#include "bloomfilter.h"
#include "queryresults.h"
#include "defs.h"
#include "ngramcounter.h"

#define STARTING_SIZE_CHILD_ARRAY 4
#define WORD_SIZE 20
#define DEFAULT_NGRAM_WORDS 10
#define DEFAULT_QUERY_BUFFER 20

typedef struct TrieNode {
    char word[WORD_SIZE];
    char *largeWord;
    struct TrieNode *children;
    int occupiedPositions;
    int capacity;
    char isFinal;
} TrieNode;

typedef struct Trie {
    TrieNode *root;
} Trie;

// This structure is used from binary_search
typedef struct SearchResults {
    char found;
    int position;
} SearchResults;

Trie *create_trie();
int destroy_trie(Trie *trie);
int insert_trie(Trie *trie, char *ngram);
void query_trie(Trie *trie, char *ngram, BloomFilter *bloomFilter, QueryResults *queryResults,
                NgramCounter *ngramCounter);
int delete_ngram_trie(Trie *trie, char *ngram);

int create_trie_node(TrieNode *trieNode);
void destroy_trie_node(TrieNode *trieNode);
void store_word_trie_node(TrieNode *trieNode, char *word);
char* get_word_trie_node(TrieNode *trieNode);
int is_empty(TrieNode *trieNode);
void delete_word_trie_node(TrieNode *trieNode, int position);
void print_trie_node(TrieNode *trieNode);

SearchResults binary_search(TrieNode *childrenArray, char *word, int occupiedPositions);
char **split_ngram(char *ngram, int *numberOfWords);
void trie_dfs_print(TrieNode *trieNode);

#endif //TRIE_H
