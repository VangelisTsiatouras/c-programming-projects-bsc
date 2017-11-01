#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "parser.h"

void myfun(){
    char tNgram[] = "test ngram";
    char tNgram2[] = "test ngram";
    char tNgram3[] = "test ngram";
    char tNgram4[] = "test ngram";
    char tNgram5[] = "test ngram";

    TrieNode *trieNode = NULL;
//    trie_node_create(trieNode, NULL);
    trieNode = malloc(sizeof(TrieNode));
    trie_node_create(trieNode, NULL);
    free(trieNode);
    Trie *trie = trie_create();
    //trie_insert(trie, tNgram);
    SearchResults se = binary_search(trie->root->children, "test", trie->root->occupiedPositions);

    // This is already unit tested
//    QueryResults *queryResults = createQueryResults(DEFAULT_LINES, DEFAULT_LINE_SIZE);
//    trie_query(trie, tNgram2, queryResults);
//    copyResultsToBufferQueryResults(queryResults);
//    flushQueryResults(queryResults);
    char nums[] = "one two three";
    char nums2[] = "one two three four";
    char nums3[] = "one two three four";
    trie_insert(trie, nums);
    trie_insert(trie, nums2);




    trie_delete_ngram(trie, nums3);

//    trie_delete_ngram(trie, tNgram3);
//    printf("%d\n", trie->root->occupiedPositions);
//    printf("%s\n", trie->root->children[0].word);
    trie_dfs_print(trie->root);
//    trie_delete_ngram(trie, nums);
//    printf("%d\n", trie->root->occupiedPositions);
//    trie_delete_ngram(trie, tNgram5);
//    printf("%d\n", trie->root->occupiedPositions);

    exit(0);
}

int main(int argc, char *argv[]) {

    //myfun();

    clock_t begin = clock();
    int i, error = 0;
    char *initFile = NULL, *queryFile = NULL;
    // Parse arguments
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) {
            i++;
            if (argv[i] != NULL) {
                initFile = argv[i];
            } else {
                error = 1;
                break;
            }
            continue;
        } else if (!strcmp(argv[i], "-q")) {
            i++;
            if (argv[i] != NULL) {
                queryFile = argv[i];
            } else {
                error = 2;
                break;
            }
            continue;
        } else {
            error = 3;
            break;
        }
    }
    if (error != 0) {
        switch (error) {
            case 1:
                printf("Error: <init_file> was expected after '-i'\n");
                break;
            case 2:
                printf("Error: <query_file> was expected after '-q'\n");
                break;
            case 3:
                printf("Error: unknown argument: %s\n", argv[i]);
                break;
        }
        printf("\nUSAGE: ./ngrams -i <init_file> -q <query_file>\n");
        printf("NOTE: flags '-i' & '-q' are required\n");
    }
    if (error != 0) {
        exit(SUCCESS);
    }
    Trie *trie = trie_create();
    parser(trie, initFile, queryFile);
    trie_destroy(trie);

    clock_t end = clock();
    double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nTime elapsed %.4f seconds\n", timeSpent);

    return SUCCESS;
}
