#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "trie.h"
#include "../linearhash/linearhash.h"
#include "../bloomfilterstorage/bfstorage.h"

Trie *create_trie() {
    Trie *trie = malloc(sizeof(Trie));
    if (!trie) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    trie->linearHash = create_LinearHash();
    return trie;
}

int destroy_trie(Trie *trie) {
    destroy_LinearHash(trie->linearHash);
    free(trie);
    return SUCCESS;
}

int insert_ngram_trie(Trie *trie, char *ngram) {
    SearchResults result;
    char *saveptr;
    char *word = strtok_r(ngram, " \n", &saveptr);
    insert_LinearHash(trie->linearHash, word);
    TrieNode *current = lookup_LinearHash(trie->linearHash, word);
    word = strtok_r(NULL, " \n", &saveptr);
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
        word = strtok_r(NULL, " \n", &saveptr);
    }
    // Mark as final
    current->isFinal = 1;
    return SUCCESS;
}

int insert_ngram_version_trie(Trie *trie, char *ngram, int version) {
    SearchResults result;
    char *saveptr;
    char *word = strtok_r(ngram, " \n", &saveptr);
    int appendManyWords = 0;
    insert_LinearHash(trie->linearHash, word);
    TrieNode *current = lookup_LinearHash(trie->linearHash, word);
//    if (current->isDeleted == 1) {
//        current->appendVersion = version;
//        current->isDeleted = 0;
//    }
    if (!strcmp(word, "absorption")){
//        printf("%s\n", word);
    }
    word = strtok_r(NULL, " \n", &saveptr);
    while (word != NULL) {
        appendManyWords = 1;
        // Don't call binary_search if the children array is empty
        if (current->occupiedPositions == 0) {
            result.position = 0;
            result.found = 0;
        } else {
            result = binary_search(current->children, word, current->occupiedPositions);
        }
        int position = result.position;

        if (result.found == 0 /*&& current->isDeleted == 0*/) {

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
            current->children[position].appendVersion = version;
            current->children[position].deleteVersion = -1;
            current->children[position].isDeleted = 0;
            current->occupiedPositions++;
        }
        if (current->appendVersion == -1 || current->isDeleted) {
            current->appendVersion = version;
        }
//        current->deleteVersion = -1;
        current->isDeleted = 0;
        current = &current->children[position];
        word = strtok_r(NULL, " \n", &saveptr);
    }
    if(appendManyWords == 0){
        if (current->appendVersion == -1 || current->isDeleted) {
            current->appendVersion = version;
        }
        current->deleteVersion = -1;
        current->isDeleted = 0;
    }

    // Mark as final
    current->markedAsFinalVersion = version;
    current->isFinal = 1;
    return SUCCESS;
}

void query_trie_dynamic(Trie *trie, char *ngram, BloomFilter *bloomFilter, QueryResults *queryResults,
                        NgramCounter *ngramCounter, int queryID, int totalQueries, int version) {
//    printf("ver %d\n",version);
    if (version == 971){
//        printf("break\n");
    }
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
    // Calculate probability of false positive based on the incoming query
    // If the probability is greater than 0.000001 the bit vector will be increased
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

        if (current->isDeleted == 1){
            continue;
        }

        offset = 0;
        resultsBuffer[0] = '\0';
        int j = i;
        do {
            if (!strcmp(splitNgram[j], "southern")) {
//                printf("%s\n", splitNgram[j]);
            }
            // Check if next word fits in resultsBuffer
            // If not realloc buffer
            size_t wordSize = strlen(splitNgram[j]) + 1;
            size_t newSize = offset + wordSize + 1;
            realloc_buffer(&resultsBuffer, &sizeBuffer, newSize);
            // Avoid overflows with offset
            // If this is not the first, add a space to separate words
            if (j != i) {
                offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, " ");
            }
            offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, "%s", splitNgram[j]);

            if (current->appendVersion <= version &&
                (current->deleteVersion > version || current->deleteVersion == -1)) {
                if (current->isFinal == 1 && (current->markedAsFinalVersion <= version || current->markedAsFinalVersion == -1)) {
                    if (check_insert_bloom_filter(bloomFilter, resultsBuffer) == SUCCESS) {
                        add_line_query_results_append(queryResults, resultsBuffer, queryID);
                        resultsFound = 1;
                        insert_ngram_counter(ngramCounter, resultsBuffer, (unsigned int) offset);
                    }
                }
            }

            j++;
            if (j < numberOfWords) {
                result = binary_search(current->children, splitNgram[j], current->occupiedPositions);
                if (result.found == 0) {
                    break;
                }
            }
            current = &current->children[result.position];
        } while (j < numberOfWords);
    }
    if (resultsFound == 0) {
        sprintf(resultsBuffer, "-1");
        add_line_query_results_append(queryResults, resultsBuffer, queryID);
    }
    free(resultsBuffer);
    free(splitNgram);
}

void query_trie_static(Trie *trie, char *ngram, BFStorage *bloomFilterStorage, QueryResults *queryResults,
                       NgramCounter *ngramCounter, int *queryID, int *totalQueries) {
    TrieNode *current;
    SearchResults result;
    BloomFilterArrayElement *bloomFilterArrayElement = obtain_filter_bf_storage(bloomFilterStorage);
    BloomFilter *bloomFilter = bloomFilterArrayElement->bloomFilter;
//    int flag = 0;//todo na fugei
//    if(!strncmp(ngram, "1 introduction in our daily lives wireless home automation networks whans promise an important role as the devices in common use can easily", 30)){
//        flag = 1;
//    }//todo na fugei
    int numberOfWords;
    char **splitNgram = split_ngram(ngram, &numberOfWords);
    int offset;
    size_t sizeBuffer = DEFAULT_QUERY_BUFFER;
    char *resultsBuffer = malloc(sizeBuffer * sizeof(char));
    if (!resultsBuffer) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    // Calculate probability of false positive based on the incoming query
    // If the probability is greater than 0.000001 the bit vector will be increased
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
            realloc_buffer(&resultsBuffer, &sizeBuffer, newSize);
            // Avoid overflows with offset
            // If this is not the first, add a space to separate words
            if (j != i) {
                offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, " ");
            }
            offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, "%s", splitNgram[j]);
            j++;
            short compressedOffset = 0;

            if (current->isFinal == 1 || (current->staticArraySize && current->staticTrieWordOffsets[0] < 0)) {
                if (check_insert_bloom_filter(bloomFilter, resultsBuffer) == SUCCESS) {
//                    if(flag == 1){//todo na fugei
//                        fprintf(stderr, "%s ", resultsBuffer);
//                    }//todo na fugei
                    add_line_query_results_append(queryResults, resultsBuffer, *queryID);
                    resultsFound = 1;
                    insert_ngram_counter(ngramCounter, resultsBuffer, (unsigned int) offset);
                }
            }
            for (int k = 1; k < current->staticArraySize; k++) {
                compressedOffset += abs(current->staticTrieWordOffsets[k - 1]);
                if (j >= numberOfWords) {
                    break;
                }
                if (compare_n_chars(current->largeWord + compressedOffset, splitNgram[j],
                                    abs(current->staticTrieWordOffsets[k])) != 0) {
                    goto breakLabel;
                }

                // Check if next word fits in resultsBuffer
                // If not realloc buffer
                wordSize = strlen(splitNgram[j]) + 1;
                newSize = offset + wordSize + 1;
                realloc_buffer(&resultsBuffer, &sizeBuffer, newSize);

                if (j != i) {
                    offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, " ");
                }
                offset += snprintf(resultsBuffer + offset, sizeBuffer - offset, "%s", splitNgram[j]);
                if (current->staticTrieWordOffsets[k] < 0) {
                    // Final
                    if (check_insert_bloom_filter(bloomFilter, resultsBuffer) == SUCCESS) {
//                        if(flag == 1){//todo na fugei
//                            fprintf(stderr, "%s ", resultsBuffer);
//                        }//todo na fugei
                        add_line_query_results_append(queryResults, resultsBuffer, *queryID);
                        resultsFound = 1;
                        insert_ngram_counter(ngramCounter, resultsBuffer, (unsigned int) offset);
                    }
                }
                j++;
            }
            if (j < numberOfWords) {
                result = binary_search(current->children, splitNgram[j], current->occupiedPositions);
                if (result.found == 0) {
                    break;
                }
            }
            current = &current->children[result.position];
        } while (j < numberOfWords);
        breakLabel:;
    }
    if (resultsFound == 0) {
        sprintf(resultsBuffer, "-1");
        add_line_query_results_append(queryResults, resultsBuffer, *queryID);
    }
    release_filter_bf_storage(bloomFilterStorage, bloomFilterArrayElement->bloomFilterID);
    free(resultsBuffer);
    free(splitNgram);
    wake_main_thread(queryResults, *totalQueries);
}

void compress_trie(Trie *trie) {
    for (int i = 0; i < trie->linearHash->arraySize; i++) {
        LHBucket *lhBucket = trie->linearHash->bucketArray[i];
        if (lhBucket != NULL) {
            for (int j = 0; j < lhBucket->occupiedPositions; j++) {
                TrieNode *trieNode = &lhBucket->nodeArray[j];
                compress_trie_node(trieNode);
            }
        }
    }
}

int delete_ngram_trie(Trie *trie, char *ngram) {
    TrieNode *current;
    LookupStruct lookupResult;
    SearchResults result;
    int numberOfWords;
    char **splitNgram = split_ngram(ngram, &numberOfWords);
    int *positionArray = malloc((numberOfWords - 1) * sizeof(int));
    if (!positionArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    TrieNode **parents = malloc((numberOfWords - 1) * sizeof(TrieNode *));
    if (!parents) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    // Iterate the trie from root and compare its words with the ngram given
    lookupResult = lookup_for_delete_LinearHash(trie->linearHash, splitNgram[0]);
    current = lookupResult.trieNode;
    if (current == NULL) {
        return DELETE_NOT_FOUND;
    }
    for (int i = 0; i < numberOfWords - 1; i++) {
        result = binary_search(current->children, splitNgram[i + 1], current->occupiedPositions);
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
    for (int i = numberOfWords - 2; i >= 0; i--) {
        // If this is the last word of the ngram
        if (i == numberOfWords - 2) {
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
    if (numberOfWords == 1 && lookupResult.trieNode->children != 0) {
        lookupResult.trieNode->isFinal = 0;
    }
    // Delete word inside the hashtable bucket
    if (lookupResult.trieNode->isFinal == 0) {
        delete_word_LHBucket(trie->linearHash->bucketArray[lookupResult.bucket], lookupResult.trieNode->word);
    }
    free(positionArray);
    free(splitNgram);
    free(parents);
    return SUCCESS;
}

int delete_ngram_version_trie(Trie *trie, char *ngram, int version) {
    TrieNode *current;
    LookupStruct lookupResult;
    SearchResults result;
    int numberOfWords;
    char **splitNgram = split_ngram(ngram, &numberOfWords);
    int *positionArray = malloc((numberOfWords - 1) * sizeof(int));
    if (!positionArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    TrieNode **parents = malloc((numberOfWords - 1) * sizeof(TrieNode *));
    if (!parents) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    // Iterate the trie from root and compare its words with the ngram given
    lookupResult = lookup_for_delete_LinearHash(trie->linearHash, splitNgram[0]);
    current = lookupResult.trieNode;
    if (current == NULL) {
        return DELETE_NOT_FOUND;
    }

    for (int i = 0; i < numberOfWords - 1; i++) {
        result = binary_search(current->children, splitNgram[i + 1], current->occupiedPositions);
        positionArray[i] = result.position;
        // If word was not found OR if it was found check if it was deleted permanently before
        if (result.found == 0 || current->isDeleted == 1) {
            free(positionArray);
            free(splitNgram);
            free(parents);
            return DELETE_NOT_FOUND;
        }
//        if (current->isDeleted == 1) {
//            return DELETE_NOT_FOUND;
//        }
        parents[i] = current;
        current = &current->children[result.position];
    }
    // If you are then the ngram was stored in the trie
    // Iterate the ngram bottom-up
    for (int i = numberOfWords - 2; i >= 0; i--) {
        // If this is the last word of the ngram
        if (i == numberOfWords - 2) {
//            current->isFinal = 0;
            current->deleteVersion = version;
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
        // Mark as permanently deleted
        current->isDeleted = 1;
        current->deleteVersion = version;
//        delete_word_trie_node(current, positionArray[i]);
    }
    if (numberOfWords == 1 && lookupResult.trieNode->children != 0) {
//        lookupResult.trieNode->isFinal = 0;
        lookupResult.trieNode->deleteVersion = version;
    }
    // Delete word inside the hashtable bucket
    if (lookupResult.trieNode->isFinal == 0) {
//        lookupResult.trieNode->deleteVersion = version;
        if (lookupResult.trieNode->occupiedPositions == 0) {
            lookupResult.trieNode->isDeleted = 1;
      }
//        delete_word_LHBucket(trie->linearHash->bucketArray[lookupResult.bucket], lookupResult.trieNode->word);
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
    trieNode->isDeleted = 0;
    trieNode->deleteVersion = -1;
    trieNode->appendVersion = -1;
    trieNode->markedAsFinalVersion = -1;
    trieNode->children = malloc(trieNode->capacity * sizeof(TrieNode));
    if (!trieNode->children) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    trieNode->staticTrieWordOffsets = NULL;
    trieNode->staticArraySize = 0;
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

void compress_trie_node(TrieNode *trieNode) {
    TrieNode *current = trieNode;
    size_t wordLength = strlen(get_word_trie_node(current)) + 1;
    // First iteration to determine the size we need to allocate
    size_t compressedWordSize = wordLength;
    unsigned int offsetsArraySize = 1;
    TrieNode *iter = current;
    while (iter != NULL && iter->occupiedPositions == 1) {
        // Go to child and copy the word to the parent
        char *childWord = get_word_trie_node(&iter->children[0]);
        compressedWordSize += strlen(childWord);
        offsetsArraySize++;
        iter = &iter->children[0];
    }
    // Now compress the nodes
    if (offsetsArraySize != 1) {
        // Allocate space for offset array
        current->staticArraySize = offsetsArraySize;
        current->staticTrieWordOffsets = malloc(current->staticArraySize * sizeof(short));
        if (!current->staticTrieWordOffsets) {
            printf("malloc error %s\n", strerror(errno));
            exit(MALLOC_ERROR);
        }

        // Allocate enough space to large word
        if (current->largeWord == NULL) {
            current->largeWord = malloc(compressedWordSize * sizeof(char));
            if (!current->largeWord) {
                printf("malloc error %s\n", strerror(errno));
                exit(MALLOC_ERROR);
            }
            strncpy(current->largeWord, current->word, wordLength);
            current->word[0] = '\0';
        } else {
            current->largeWord = realloc(current->largeWord, compressedWordSize * sizeof(char));
            if (!current->largeWord) {
                printf("realloc error %s\n", strerror(errno));
                exit(REALLOC_ERROR);
            }
        }
        current->staticTrieWordOffsets[0] = (short) (wordLength - 1);
        // Mark finals as negative values in the staticTrieWordOffsets array
        if (current->isFinal == 1) {
            current->staticTrieWordOffsets[0] *= -1;
        }

        int i = 1;
        while (current->occupiedPositions == 1) {
            // Go to child and copy the word to the parent
            char *childWord = get_word_trie_node(&current->children[0]);
            TrieNode *temp = &current->children[0];
            size_t childWordLength = strlen(childWord) + 1;
            wordLength += childWordLength;
            strncat(current->largeWord, childWord, wordLength);
            current->staticTrieWordOffsets[i] = (short) (childWordLength - 1);
            if (temp->isFinal == 1) {
                current->staticTrieWordOffsets[i] *= -1;
            }
            i++;

            // Copy the children of the child to the parent
            current->children = temp->children;
            current->occupiedPositions = temp->occupiedPositions;

            // Delete child node
            if (temp->largeWord != NULL) {
                free(temp->largeWord);
            }
            if (temp->staticTrieWordOffsets != NULL) {
                free(temp->staticTrieWordOffsets);
            }
            free(temp);
        }
    }

    // For each children of the compressed node do recursion
    for (int i = 0; i < current->occupiedPositions; i++) {
        compress_trie_node(&current->children[i]);
    }
}

void destroy_trie_node(TrieNode *trieNode) {
    for (int i = 0; i < trieNode->occupiedPositions; i++) {
        destroy_trie_node(&trieNode->children[i]);
    }
    if (trieNode->largeWord != NULL) {
        free(trieNode->largeWord);
    }
    if (trieNode->staticTrieWordOffsets != NULL) {
        free(trieNode->staticTrieWordOffsets);
    }
    free(trieNode->children);
}

char *get_word_trie_node(TrieNode *trieNode) {
    if (trieNode->largeWord == NULL) {
        return trieNode->word;
    }
    return trieNode->largeWord;
}

int is_empty_trie_node(TrieNode *trieNode) {
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
        // If node is compressed
        if (childrenArray[middle].staticArraySize > 0) {
            strcmp_result = compare_n_chars(nodeWord, word, abs(childrenArray[middle].staticTrieWordOffsets[0]));
            // If node is not compressed
        } else {
            strcmp_result = strcmp(nodeWord, word);
        }
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
    char *saveptr;
    char *word = strtok_r(ngram, " \n", &saveptr);
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
        word = strtok_r(NULL, " \n", &saveptr);
    }
    return splitNgram;
}

void realloc_buffer(char **buffer, size_t *sizeBuffer, size_t newSize) {
    if (newSize > *sizeBuffer) {
        *sizeBuffer *= 2;
        if (newSize > *sizeBuffer) {
            *sizeBuffer = newSize;
        }
        *buffer = realloc(*buffer, *sizeBuffer * sizeof(char));
        if (!*buffer) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
    }
}

int compare_n_chars(char *buffer1, char *buffer2, int n) {
    char *tempNodeWord = malloc((n + 1) * sizeof(char));
    if (!tempNodeWord) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    strncpy(tempNodeWord, buffer1, (size_t) n + 1);
    tempNodeWord[n] = '\0';
    int strcmp_result = strcmp(tempNodeWord, buffer2);
    free(tempNodeWord);
    return strcmp_result;
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

void tester_compress() {
    char tNgram5[100] = "this should be compressed to one node";
    char tNgram6[100] = "this should be compressed to another node";
    char tNgram7[100] = "this should";

    Trie *trie = create_trie();

    // Check compress
    insert_ngram_version_trie(trie, tNgram5, 0);
    insert_ngram_version_trie(trie, tNgram6, 0);
    insert_ngram_version_trie(trie, tNgram7, 0);
    compress_trie_node(&trie->linearHash->bucketArray[0]->nodeArray[0]);

    destroy_trie(trie);
    exit(0);
}
