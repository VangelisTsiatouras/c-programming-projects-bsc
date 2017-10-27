#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "trie.h"

Trie *trie_create() {
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

int trie_destroy(Trie *trie) {
    trie_node_destroy(trie->root);
    free(trie->root);
    free(trie);
}

int trie_insert(Trie *trie, char *ngram) {

    TrieNode *current = trie->root;
    SearchResults result;
    char *word = strtok(ngram, " \n");
    while (word != NULL) {
//        printf("%s\n", word);
        // Don't call binary_search if the children array is empty
        if (current->occupiedPositions == 0) {
            result.position = 0;
            result.found = 0;
        } else {
            result = binary_search(current->children, word, current->occupiedPositions);
        }
//        printf("Binary search returned pos: %d\n", result.position);
        int position = result.position;
        if (result.found == 0) {
            // Reallocate space if the children array is full
            if (current->occupiedPositions == current->capacity) {
                // The new size will be the double of the old size
                current->capacity *= 2;
                TrieNode *tempChildren = realloc(current->children, current->capacity * sizeof(TrieNode));
                if (tempChildren == NULL) {
                    printf("realloc error %s\n", strerror(errno));
                    exit(-1);
                }
                current->children = tempChildren;
            }
            // Shift elements to keep the children array sorted
            if (position < current->occupiedPositions) {
//                printf("shifting %d positions\n", (current->occupiedPositions - position));
                memmove(&current->children[position + 1], &current->children[position],
                        sizeof(TrieNode) * (current->occupiedPositions - position));
            }
            trie_node_create(&current->children[position], current);
//            printf("Creating node with word: %s\n", word);
            strncpy(current->children[position].word, word, WORD_SIZE);
            current->occupiedPositions++;
        }

        current = &current->children[position];

        word = strtok(NULL, " \n");
    }
    // Mark as final
    current->isFinal = 1;
}

void trie_query(Trie *trie, char *ngram, QueryResults *queryResults) {
    TrieNode *current;
    SearchResults result;
    int i, j;
    int numberOfWords;
    char **ngramSplitted = split_ngram(ngram, &numberOfWords);

    int offset;
    size_t sizeBuffer = (size_t) numberOfWords * WORD_SIZE;
    char *resultsBuffer = malloc(sizeBuffer * sizeof(char));
    if (!resultsBuffer) {
        printf("malloc error %s\n", strerror(errno));
        exit(1);
    }

    int resultsFound = 0;
    // Iterate the ngram word by word
    for (i = 0; i < numberOfWords; i++) {
        current = trie->root;
        offset = 0;
        resultsBuffer[0] = '\0';
        for (j = i; j < numberOfWords; j++) {
            result = binary_search(current->children, ngramSplitted[j], current->occupiedPositions);
            if (result.found == 0) {
                break;
            }
            current = &current->children[result.position];

            // Avoid overflows with offset
            // If this is not the first, add a space to separate words
            if (j != i) {
                offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, " ");
            }
            offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, "%s", ngramSplitted[j]);
            if (current->isFinal == 1) {
                addLineQueryResults(queryResults, resultsBuffer);
                resultsFound = 1;
            }
        }
    }
    if (resultsFound == 0) {
        sprintf(resultsBuffer, "-1");
        addLineQueryResults(queryResults, resultsBuffer);
    }
    free(resultsBuffer);
    free(ngramSplitted);
}

void trie_delete_ngram(Trie *trie, char *ngram) {
    TrieNode *current;
    SearchResults result;
    int i, j;
    int numberOfWords;
    char **ngramSplitted = split_ngram(ngram, &numberOfWords);
    int *positionArray = malloc(numberOfWords * sizeof(int));
    if (!positionArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }

    // Iterate the trie from root and compare its words with the ngram given
    current = trie->root;
    for (i = 0; i < numberOfWords; i++) {
        result = binary_search(current->children, ngramSplitted[i], current->occupiedPositions);
        positionArray[i] = result.position;
        if (result.found == 0) {
            //printf("ngram not found\n"); //todo return int code
            free(positionArray);
            free(ngramSplitted);
            return;
        }
        current = &current->children[result.position];
    }
    // If you are then the ngram was stored in the trie
    // Iterate the ngram bottom-up
    for (i = numberOfWords - 1; i >= 0; i--) {
        // If you found a node that has children or is final, return
        if (current->occupiedPositions > 0 || current->isFinal == 1) {
            free(positionArray);
            free(ngramSplitted);
            current->isFinal = 0;
            return;
        }
        current = current->parent;
        trie_node_delete_word(current, positionArray[i]);
    }
    free(positionArray);
    free(ngramSplitted);
}

int trie_node_create(TrieNode *trieNode, TrieNode *parent) {
    trieNode->capacity = STARTING_SIZE_CHILD_ARRAY;
    trieNode->occupiedPositions = 0;
    trieNode->parent = parent;
    trieNode->isFinal = 0;
    trieNode->children = malloc(trieNode->capacity * sizeof(TrieNode));
    if (!trieNode->children) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    return 0;
}

int trie_node_destroy(TrieNode *trieNode) {
    int i;
    for (i = 0; i < trieNode->occupiedPositions; ++i) {
        trie_node_destroy(&trieNode->children[i]);
    }
    free(trieNode->children);
}

int trie_node_delete_word(TrieNode *trieNode, int position) {
    // Free the node
    trie_node_destroy(&trieNode->children[position]);
    // If this isn't the last element in the children array
    if (position < trieNode->occupiedPositions) {
        // Shift elements to the left
        memmove(&trieNode->children[position], &trieNode->children[position + 1],
                sizeof(TrieNode) * (trieNode->occupiedPositions - position - 1));
    }
    trieNode->occupiedPositions--;
}

void trie_node_print(TrieNode *trieNode) {
    int i;
    if (trieNode->word != NULL) { //todo check this null compare for uninitialized values
        printf("Node with word: %s\n", trieNode->word);
    }
    for (i = 0; i < trieNode->occupiedPositions; i++) {
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
    // If the array is empty then return "not found"
    if (occupiedPositions == 0) {
        results.found = 0;
        results.position = -1;
        return results;
    }
    while (left <= right) {
        middle = (left + right) / 2;
        strncmp_result = strncmp(childrenArray[middle].word, word, WORD_SIZE);
//        printf("%s\n", childrenArray[middle].word);
        if (strncmp_result < 0) {
            left = middle + 1;
            continue;
        } else if (strncmp_result > 0) {
            right = middle - 1;
            continue;
        } else {
            results.found = 1;
            break;
        }
    }
    results.position = middle;

    // Check if the word of middle is less than the target
    // If it is true, increase the target position
    if (strncmp(childrenArray[middle].word, word, WORD_SIZE) < 0) {
        results.position++;
    }
    return results;
}

char **split_ngram(char *ngram, int *numberOfWords) {
    // Split the ngram to words and store them to array
    int maxSize = DEFAULT_NGRAM_WORDS;
    *numberOfWords = 0;
    char **ngramSplitted = malloc(sizeof(char *) * maxSize);
    if (!ngramSplitted) {
        printf("malloc error %s\n", strerror(errno));
        exit(1);
    }
    char *word = strtok(ngram, " \n");
    while (word != NULL) {
        (*numberOfWords)++;
        if (*numberOfWords == maxSize) {
            maxSize *= 2;
            ngramSplitted = realloc(ngramSplitted, sizeof(char *) * maxSize);
            if (!ngramSplitted) {
                printf("realloc error %s\n", strerror(errno));
                exit(1);
            }
        }
        ngramSplitted[*numberOfWords - 1] = word;
        word = strtok(NULL, " \n");
    }
    return ngramSplitted;
}

void trie_dfs(TrieNode *trieNode) {
    int i;
    printf("%s ", trieNode->word);
    if (trieNode->isFinal == 1) {
        printf("FINAL!");
    }
    printf("\n");
    for (i = 0; i < trieNode->occupiedPositions; i++) {
        trie_dfs(&trieNode->children[i]);
    }
}

//TODO LIST
//todo 1 use buffer sthn trie_query
//todo 2 use hybrid string gia to word
