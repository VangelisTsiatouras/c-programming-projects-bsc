#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "trie.h"

Trie* trie_create() {
    printf("trie init\n");
    Trie *trie = malloc(sizeof(Trie));
    trie->root = trie_node_create();
    return trie;
}

int trie_delete(Trie *trie) {
    free(trie);
}

int trie_insert(Trie *trie, char *newWord) {

}

TrieNode* trie_node_create() {
    TrieNode *trieNode = malloc(sizeof(TrieNode));
    if (!trieNode) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    trieNode->capacity = STARTING_SIZE_CHILD_ARRAY;
    trieNode->occupiedPositions = 0;
    trieNode->children = malloc(trieNode->capacity * sizeof(TrieNode));
    if (!trieNode->children) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    return trieNode;
}

int trie_node_delete(TrieNode *trieNode) {
    free(trieNode->children);
    free(trieNode);
}