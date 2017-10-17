#ifndef TRIE_H
#define TRIE_H

#define STARTING_SIZE_CHILD_ARRAY 4
#define WORD_SIZE 32

typedef struct TrieNode {
    char word[WORD_SIZE];
    struct TrieNode *children;
    struct TrieNode *parent;
    int occupiedPositions;
    int capacity;
    char isFinal;
}TrieNode;

typedef struct Trie {
    TrieNode *root;
}Trie;

Trie* trie_create();
int trie_delete(Trie *trie);
int trie_insert(Trie *trie, char *ngram);

int trie_node_create(TrieNode *trieNode, TrieNode *parent);
int trie_node_delete(TrieNode *trieNode);

#endif //TRIE_H
