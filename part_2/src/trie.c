#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "trie.h"
#include "linearhash.h"

Trie *create_trie() {
    Trie *trie = malloc(sizeof(Trie));
    if (!trie) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
//    trie->root = malloc(sizeof(TrieNode));
//    if (!trie->root) {
//        printf("malloc error %s\n", strerror(errno));
//        exit(MALLOC_ERROR);
//    }
//    create_trie_node(trie->root);
    trie->linearHash = create_LinearHash();
    return trie;
}

int destroy_trie(Trie *trie) {
//    destroy_trie_node(trie->root);
//    free(trie->root);
    destroy_LinearHash(trie->linearHash);
    free(trie);
    return SUCCESS;
}

int insert_trie(Trie *trie, char *ngram) {
    SearchResults result;
    char *word = strtok(ngram, " \n");
    insert_LinearHash(trie->linearHash, word);
    TrieNode *current = lookup_LinearHash(trie->linearHash, word); // todo gt na kanoume insert kai meta lookup....
    word = strtok(NULL, " \n");
    while (word != NULL) {
        // Don't call binary_search if the children array is empty
        if (current->occupiedPositions == 0) {
            result.position = 0;
            result.found = 0;
        } else {
            result = binary_search(current->children, word, current->occupiedPositions);
        }
        int position = result.position;
        if (result.found == 0) {
            // Reallocate space if the children array is full
            if (current->occupiedPositions == current->capacity) {
                // The new size will be the double of the old size
                current->capacity *= 2;
                TrieNode *tempChildren = realloc(current->children, current->capacity * sizeof(TrieNode));
                if (tempChildren == NULL) {
                    printf("realloc error %s\n", strerror(errno));
                    exit(REALLOC_ERROR);
                }
                current->children = tempChildren;
            }
            // Shift elements to keep the children array sorted
            if (position < current->occupiedPositions) {
                memmove(&current->children[position + 1], &current->children[position],
                        sizeof(TrieNode) * (current->occupiedPositions - position));
            }
            create_trie_node(&current->children[position]);
            store_word_trie_node(&current->children[position], word);
            current->occupiedPositions++;
        }
        current = &current->children[position];
        word = strtok(NULL, " \n");
    }
    // Mark as final
    current->isFinal = 1;
    return SUCCESS;
}

void query_trie(Trie *trie, char *ngram, BloomFilter *bloomFilter, QueryResults *queryResults,
                NgramCounter *ngramCounter) {
    TrieNode *current;
    SearchResults result;
    int numberOfWords;
    char **splitNgram = split_ngram(ngram, &numberOfWords);
    int offset;
    size_t sizeBuffer = DEFAULT_QUERY_BUFFER;
    char *resultsBuffer = malloc(sizeBuffer * sizeof(char));
    if (!resultsBuffer) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    // Calculate probality of false positive based on the incoming query
    // If the probabilty is greater than 0.0001 the bit vector will be increased
    probability_of_query_bloom_filter(bloomFilter, numberOfWords);
    // Set to zero bit vector
    set_to_zero_bloom_filter(bloomFilter);
    int resultsFound = 0;
    // Iterate the ngram word by word
    for (int i = 0; i < numberOfWords; i++) {
        current = lookup_LinearHash(trie->linearHash, splitNgram[i]);
        if (current == NULL) {
            continue;
        }
        offset = 0;
        resultsBuffer[0] = '\0';
        int j = i;
        do {
            // Check if next word fits in resultsBuffer
            // If not realloc buffer
            size_t wordSize = strlen(splitNgram[j]) + 1;
            size_t newSize = offset + wordSize + 1;
            if (newSize > sizeBuffer) {
                sizeBuffer *= 2;
                if (newSize > sizeBuffer) {
                    sizeBuffer = newSize;
                }
                resultsBuffer = realloc(resultsBuffer, sizeBuffer * sizeof(char));
                if (!resultsBuffer) {
                    printf("realloc error %s\n", strerror(errno));
                    exit(REALLOC_ERROR);
                }

            }
            // Avoid overflows with offset
            // If this is not the first, add a space to separate words
            if (j != i) {
                offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, " ");
            }
            offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, "%s", splitNgram[j]);
            if (current->isFinal == 1) {
                if (check_insert_bloom_filter(bloomFilter, resultsBuffer) == SUCCESS) {
                    add_line_query_results_append(queryResults, resultsBuffer);
                    resultsFound = 1;
                    insert_ngram_counter(ngramCounter, resultsBuffer);
                }
            }
            j++;
            if( j < numberOfWords) {
                result = binary_search(current->children, splitNgram[j], current->occupiedPositions);
                if (result.found == 0) {
                    break;
                }
            }
            current = &current->children[result.position];
        } while(j < numberOfWords);
    }
    if (resultsFound == 0) {
        sprintf(resultsBuffer, "-1");
        add_line_query_results_append(queryResults, resultsBuffer);
    }
    free(resultsBuffer);
    free(splitNgram);
}

int delete_ngram_trie(Trie *trie, char *ngram) {
    TrieNode *current;
    SearchResults result;
    int numberOfWords;
    char **splitNgram = split_ngram(ngram, &numberOfWords);
    int *positionArray = malloc(numberOfWords * sizeof(int));
    if (!positionArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    TrieNode **parents = malloc(numberOfWords * sizeof(TrieNode *));
    if (!parents) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    // Iterate the trie from root and compare its words with the ngram given
    current = trie->linearHash; //TODO AUTO NA ALLA3EI.
    for (int i = 0; i < numberOfWords; i++) {
        result = binary_search(current->children, splitNgram[i], current->occupiedPositions);
        positionArray[i] = result.position;
        if (result.found == 0) {
            free(positionArray);
            free(splitNgram);
            free(parents);
            return DELETE_NOT_FOUND;
        }
        parents[i] = current;
        current = &current->children[result.position];
    }
    // If you are then the ngram was stored in the trie
    // Iterate the ngram bottom-up
    for (int i = numberOfWords - 1; i >= 0; i--) {
        // If this is the last word of the ngram
        if (i == numberOfWords - 1) {
            current->isFinal = 0;
            // If it has children
            if (current->occupiedPositions > 0) {
                free(positionArray);
                free(splitNgram);
                free(parents);
                return SUCCESS;
            }
        }
        // If it has children or if it is a final state
        if (current->occupiedPositions > 0 || current->isFinal == 1) {
            free(positionArray);
            free(splitNgram);
            free(parents);
            return SUCCESS;

        }
        current = parents[i];
        delete_word_trie_node(current, positionArray[i]);
    }
    free(positionArray);
    free(splitNgram);
    free(parents);
    return SUCCESS;
}

int create_trie_node(TrieNode *trieNode) {
    if (trieNode == NULL) {
        return NOT_ALLOCATED_ERROR;
    }
    trieNode->word[0] = '\0';
    trieNode->largeWord = NULL;
    trieNode->capacity = STARTING_SIZE_CHILD_ARRAY;
    trieNode->occupiedPositions = 0;
    trieNode->isFinal = 0;
    trieNode->children = malloc(trieNode->capacity * sizeof(TrieNode));
    if (!trieNode->children) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    return SUCCESS;
}

void store_word_trie_node(TrieNode *trieNode, char *word) {
    size_t newWordLength = strlen(word) + 1;
    if (newWordLength > WORD_SIZE) {
        trieNode->largeWord = malloc(newWordLength * sizeof(char));
        if (!trieNode->largeWord) {
            printf("malloc error %s\n", strerror(errno));
            exit(MALLOC_ERROR);
        }
        strncpy(trieNode->largeWord, word, newWordLength);
    } else {
        strncpy(trieNode->word, word, WORD_SIZE);
    }
}

void destroy_trie_node(TrieNode *trieNode) {
    for (int i = 0; i < trieNode->occupiedPositions; i++) {
        destroy_trie_node(&trieNode->children[i]);
    }
    if (trieNode->largeWord != NULL) {
        free(trieNode->largeWord);
    }
    free(trieNode->children);
}

char *get_word_trie_node(TrieNode *trieNode) {
    if (trieNode->largeWord == NULL) {
        return trieNode->word;
    }
    return trieNode->largeWord;
}

int is_empty(TrieNode *trieNode) {
    if (trieNode->largeWord == NULL && trieNode->word[0] == '\0') {
        return 1;
    }
    return 0;
}

void delete_word_trie_node(TrieNode *trieNode, int position) {
    // Free the node
    destroy_trie_node(&trieNode->children[position]);
    // If this isn't the last element in the children array
    if (position < trieNode->occupiedPositions - 1) {
        // Shift elements to the left
        memmove(&trieNode->children[position], &trieNode->children[position + 1],
                sizeof(TrieNode) * (trieNode->occupiedPositions - position - 1));
    }
    trieNode->occupiedPositions--;
}

void print_trie_node(TrieNode *trieNode) {
    char *word = get_word_trie_node(trieNode);
    if (word != NULL) {
        printf("Node with word: %s\n", word);
    }
    for (int i = 0; i < trieNode->occupiedPositions; i++) {
        word = get_word_trie_node(trieNode);
        printf("child %d: %s\n", i, word);
    }
}

SearchResults binary_search(TrieNode *childrenArray, char *word, int occupiedPositions) {
    SearchResults results;
    int strcmp_result;
    int left, right, middle = 0;
    char *nodeWord = NULL;
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
        nodeWord = get_word_trie_node(&childrenArray[middle]);
        strcmp_result = strcmp(nodeWord, word);
        if (strcmp_result < 0) {
            left = middle + 1;
            continue;
        } else if (strcmp_result > 0) {
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
    if (strcmp(nodeWord, word) < 0) {
        results.position++;
    }
    return results;
}

char **split_ngram(char *ngram, int *numberOfWords) {
    // Split the ngram to words and store them to array
    int maxSize = DEFAULT_NGRAM_WORDS;
    *numberOfWords = 0;
    char **splitNgram = malloc(sizeof(char *) * maxSize);
    if (!splitNgram) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    char *word = strtok(ngram, " \n");
    while (word != NULL) {
        (*numberOfWords)++;
        if (*numberOfWords == maxSize) {
            maxSize *= 2;
            splitNgram = realloc(splitNgram, sizeof(char *) * maxSize);
            if (!splitNgram) {
                printf("realloc error %s\n", strerror(errno));
                exit(REALLOC_ERROR);
            }
        }
        splitNgram[*numberOfWords - 1] = word;
        word = strtok(NULL, " \n");
    }
    return splitNgram;
}

void trie_dfs_print(TrieNode *trieNode) {
    char *word = get_word_trie_node(trieNode);
    printf("%-20s ", word);
    if (trieNode->isFinal == 1) {
        printf("FINAL");
    }
    printf("\n");
    for (int i = 0; i < trieNode->occupiedPositions; i++) {
        trie_dfs_print(&trieNode->children[i]);
    }
}
