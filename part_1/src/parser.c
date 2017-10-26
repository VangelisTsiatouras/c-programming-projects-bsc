#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "parser.h"

int parser(Trie *trie, char *initFile, char *queryFile) {
    FILE *iFile, *qFile;
    iFile = fopen(initFile, "r");
    if(iFile == NULL) {
        printf("fopen error %s\n", strerror(errno));
        exit(-1);
    }
    qFile = fopen(queryFile, "r");
    if(qFile == NULL) {
        printf("fopen error %s\n", strerror(errno));
        exit(-1);
    }
    char* line = NULL;
    size_t lineSize = 0;
    while (getline(&line, &lineSize, iFile) > 0) {
        //printf("A %s", line);
        trie_insert(trie, line);
    }
    printf("##################\n");
//    char uni[] = "single";
//    trie_delete_ngram(trie, &line[2]);
//    trie_query(trie, uni);
//    trie_node_print(trie->root);
    while (getline(&line, &lineSize, qFile) > 0) {
        switch(line[0]) {
            case 'Q':
                //printf("%s", line);
                trie_query(trie, &line[2]);
                break;
            case 'A':
                trie_insert(trie, &line[2]);
                //printf("A\n");
                break;
            case 'D':
                //printf("%s", line);
                trie_delete_ngram(trie, &line[2]);

                break;
            case 'F':
                //printf("F\n");
                break;
            default:
                printf("default\n");

        }
        //printf("%ld %s", lineSize, line);
    }
    free(line);

    fclose(iFile);
    fclose(qFile);
    return 0;
}
