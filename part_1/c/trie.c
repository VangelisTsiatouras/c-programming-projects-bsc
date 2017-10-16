#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "trie.h"

Trie* trie_create() {
    printf("trie init\n");
    Trie *trie = malloc(sizeof(Trie));
    trie->root = malloc(sizeof(TrieNode));
    if (!trie->root) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    trie->root->capacity = STARTING_SIZE_CHILD_ARRAY;
    trie->root->occupiedPositions = 0;
    trie->root->children = malloc(trie->root->capacity * sizeof(TrieNode));
    if (!trie->root->children) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    return trie;
}

int trie_delete(Trie *trie) {
    free(trie->root);
    free(trie->root->children);
}

int trie_insert(Trie *trie, char *newWord) {

}
