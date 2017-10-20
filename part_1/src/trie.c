#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "trie.h"

Trie *trie_create() {
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
    int i;
    SearchResults result;
    char *word = strtok(ngram, " ");
    while (word != NULL) {
        printf("%s\n", word);

        SearchResults result = binary_search(current->children, word, current->occupiedPositions);
        int position = result.position;
        if (result.found == 0) {
            // Reallocate space if the children array is full
            if (current->occupiedPositions == current->capacity) {
                // The new size will be the double of the old size
                printf("reallocating\n");
                current->capacity *= 2;
                TrieNode *tempChildren = (TrieNode *) realloc(current->children, current->capacity * sizeof(TrieNode));
                if (tempChildren == NULL) {
                    printf("realloc error %s\n", strerror(errno));
                    exit(-1);
                }
                current->children = tempChildren;
            }
            // Shift elements to keep the children array sorted
            if (position < current->occupiedPositions) {
                printf("shifting %d positions\n", (current->occupiedPositions - position));
                memmove(&current->children[position + 1], &current->children[position],
                        sizeof(TrieNode) * (current->occupiedPositions - position));
            }
            trie_node_create(&current->children[position], current);
            printf("Creating node with word: %s\n", word);
            strncpy(current->children[position].word, word, WORD_SIZE);
            current->occupiedPositions++;
        }

        current = &current->children[position];

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

void trie_node_print(TrieNode *trieNode) {
    int i;
    if(trieNode->word != NULL) {
        printf("Node with word: %s\n", trieNode->word);
    }
    for(i = 0; i < trieNode->occupiedPositions; i++) {
        printf("child %d: %s\n", i, trieNode->children[i].word);
    }
}

SearchResults binary_search(TrieNode *childrenArray, char *word, int occupiedPositions) {
    SearchResults results;
    int strncmp_result;
    int left, right, middle = 0;
    results.found = 0;
    left = 0;
    right = occupiedPositions - 1;
    // If occupiedPositions is 0, don't initialize right with negative value!
    right = right < 0 ? 0 : right;
    while (left <= right) {
        middle = (left + right) / 2;
        strncmp_result = strncmp(childrenArray[middle].word, word, WORD_SIZE);
        if (strncmp_result < 0) {
            printf("less than\n");
            left = middle + 1;
            continue;
        } else if (strncmp_result > 0) {
            printf("greater than\n");
            right = middle - 1;
            continue;
        }
        results.found = 1;
        break;
    }
    results.position = middle;
//    strncmp_result = strncmp(childrenArray[middle].word, word, WORD_SIZE);
//    if(strncmp_result > 0) {
//        results.position--;
//    }
//    } else if(strncmp_result < 0) {
//        results.position++;
//    }
    return results;
}
