#ifndef TRIE_H
#define TRIE_H

#define STARTING_SIZE_CHILD_ARRAY 4

typedef struct TrieNode {
    char word[32];
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
int trie_insert(Trie *trie, char *newWord);

TrieNode* trie_node_create();
int trie_node_delete(TrieNode *trieNode);

#endif //TRIE_H
