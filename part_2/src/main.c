#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "parser/parser.h"

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
        exit(SUCCESS);
    }
    Trie *trie = create_trie();
    parser(trie, initFile, queryFile);
    destroy_trie(trie);

    clock_t end = clock();
    double timeSpent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("\nTime elapsed %.4f seconds\n", timeSpent);

    return SUCCESS;
}
