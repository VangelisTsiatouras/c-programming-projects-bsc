#ifndef TRIE_H
#define TRIE_H

#include "queryresults.h"

#define STARTING_SIZE_CHILD_ARRAY 4
#define WORD_SIZE 16
#define DEFAULT_NGRAM_WORDS 10

typedef struct TrieNode {
    char word[WORD_SIZE];
    char *largeWord;
    struct TrieNode *children;
    struct TrieNode *parent;
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

Trie *trie_create();

int trie_destroy(Trie *trie);

int trie_insert(Trie *trie, char *ngram);

void trie_query(Trie *trie, char *ngram, QueryResults *queryResults);

void trie_delete_ngram(Trie *trie, char *ngram);

int trie_node_create(TrieNode *trieNode, TrieNode *parent);

int trie_node_destroy(TrieNode *trieNode);

char* trie_node_get_word(TrieNode *trieNode);

int trie_node_delete_word(TrieNode *trieNode, int position);

void trie_node_print(TrieNode *trieNode);

SearchResults binary_search(TrieNode *childrenArray, char *word, int occupiedPositions);

char **split_ngram(char *ngram, int *numberOfWords);

void trie_dfs_print(TrieNode *trieNode);

#endif //TRIE_H
