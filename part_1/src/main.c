#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "parser.h"
#include "queryresults.h" // todo na fugei sto teliko

int main(int argc, char *argv[]) {

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
        exit(0);
    }
    Trie *trie = trie_create();
    parser(trie, initFile, queryFile);
    trie_destroy(trie);

    clock_t end = clock();
    double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time elapsed %.4f seconds\n", timeSpent);

    return 0;
}

///////////////////////////////////////////////////////////////
//                THE BELOW ARE USED FOR THE DEBUG
///////////////////////////////////////////////////////////////

//char assignment_ngram1[] = "this is a dog";
//char assignment_ngram2[] = "this is a cat";
//char assignment_ngram3[] = "the fox is bad";
//char assignment_ngram4[] = "the car is red";
//char assignment_ngram5[] = "this is a test";
//char assignment_ngram6[] = "this is a";
//char assignment_ngram7[] = "test";
//char assignment_ngram8[] = "fast car";
//
//char assignment_query1[] = "this is a cat and a dog";
//char assignment_query2[] = "this is not the test";
//char assignment_query3[] = "the car is red";
//char assignment_query4[] = "the car is red and fast";
//
//Trie *trie = trie_create();
//
//trie_insert(trie, assignment_ngram1);
//trie_insert(trie, assignment_ngram2);
//trie_insert(trie, assignment_ngram3);
//trie_insert(trie, assignment_ngram4);
//trie_insert(trie, assignment_ngram5);
//trie_insert(trie, assignment_ngram6);
//
//trie_query(trie, assignment_query1);
//trie_query(trie, assignment_query2);
//
//trie_insert(trie, assignment_ngram7);
//
//char assignment_query2COPY[] = "this is not the test";
//trie_query(trie, assignment_query2COPY);
//
//trie_insert(trie, assignment_ngram8);
//
//trie_delete_ngram(trie, assignment_query3);
//
//trie_query(trie, assignment_query4);
//
//
//trie_destroy(trie);
//printf("exiting..!\n");