#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "trie.h"

Trie* trie_create() {
    printf("trie init\n");
    Trie *trie = malloc(sizeof(Trie));
    if (!trie) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    trie->root = malloc(sizeof(TrieNode));
    if (!trie->root) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    trie_node_create(trie->root, NULL);
    return trie;
}

int trie_delete(Trie *trie) {
    free(trie->root);
    free(trie);
}

//todo add binary search
int trie_insert(Trie *trie, char *ngram) {
    TrieNode *current = trie->root;
    int i, result = 1;
    char *word = strtok (ngram, " ");
    while(word != NULL) {
        printf("%s\n", word);
        for(i = 0; i < current->occupiedPositions; i++) {
            result = strncmp(current->children[i].word, word, WORD_SIZE);
            if(result < 0) {
                printf("less than\n");
                continue;
            } else if(result == 0){
                printf("equal\n");
                break;
            } else {
                printf("greater than\n");
                break;
            }
        }
        if (result != 0) {
            // Reallocate space if the children array is full
            if(current->occupiedPositions == current->capacity) {
                // The new size will be the double of the old size
                printf("reallocating\n");
                current->capacity *= 2;
                TrieNode *tempChildren = (TrieNode*)realloc(current->children, current->capacity * sizeof(TrieNode));
                if (tempChildren ==NULL) {
                    printf("realloc error %s\n", strerror(errno));
                    exit(-1);
                }
                current->children=tempChildren;
            }
            // Shift elements to keep the children array sorted
            if(i < current->occupiedPositions) {
                printf("shifting %d positions\n",(current->occupiedPositions - i));
                memmove(&current->children[i+1], &current->children[i], sizeof(TrieNode) * (current->occupiedPositions - i));
            }
            trie_node_create(&current->children[i], current);
            printf("Creating node with word: %s\n", word);
            strncpy(current->children[i].word, word, WORD_SIZE);
            current->occupiedPositions++;
        }

        current = &current->children[i];

        word = strtok(NULL, " ");
    }
}

int trie_node_create(TrieNode *trieNode, TrieNode *parent) {
    trieNode->capacity = STARTING_SIZE_CHILD_ARRAY;
    trieNode->occupiedPositions = 0;
    trieNode->parent = parent;
    trieNode->children = malloc(trieNode->capacity * sizeof(TrieNode));
    if (!trieNode->children) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    return 0;
}

int trie_node_delete(TrieNode *trieNode) {
    free(trieNode->children);
}