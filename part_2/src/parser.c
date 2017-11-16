#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "queryresults.h"
#include "ngramcounter.h"
#include "parser.h"

int parser(Trie *trie, char *initFile, char *queryFile) {
    FILE *iFile, *qFile;
    iFile = fopen(initFile, "r");
    if (iFile == NULL) {
        printf("fopen error %s\n", strerror(errno));
        exit(FOPEN_ERROR);
    }
    qFile = fopen(queryFile, "r");
    if (qFile == NULL) {
        printf("fopen error %s\n", strerror(errno));
        exit(FOPEN_ERROR);
    }
    BloomFilter *bloomFilter = bloom_filter_create();
    QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
    NgramCounter* ngramCounter = create_ngram_counter();
    char *line = NULL;
    char *word = NULL;
    int topk = 3;
    size_t lineSize = 0;
    while (getline(&line, &lineSize, iFile) > 0) {
        trie_insert(trie, line);
    }
    while (getline(&line, &lineSize, qFile) > 0) {
        switch (line[0]) {
            case 'Q':
                bloom_filter_set_to_zero(bloomFilter);
                trie_query(trie, &line[2], bloomFilter, queryResults, ngramCounter);
                copy_results_to_buffer_query_results(queryResults);
                break;
            case 'A':
                trie_insert(trie, &line[2]);
                break;
            case 'D':
                trie_delete_ngram(trie, &line[2]);

                break;
            case 'F':
                word = strtok(line + 1, " \n");
                if (word != NULL ){
                    topk = atoi(word);
                    if (topk < 1) {
                        printf("Invalid top k given: %d\n", topk);
                        exit(-1); //todo swsto error code
                    }
                    NgramArray *ngramArray = copy_to_ngram_array(ngramCounter, ngramCounter->elements);
                    sort_topk(ngramArray, (unsigned int)topk);
                    print_ngram_array(ngramArray);
                    destroy_ngram_array(ngramArray);
                }
                clear_ngram_counter(ngramCounter);
                flush_query_results(queryResults);
                break;
            default:
                printf("default\n");//todo na valoume swsto error message

        }
    }
    destroy_gram_counter(ngramCounter);
    free(line);
    fclose(iFile);
    fclose(qFile);
    destroy_query_results(queryResults);
    bloom_filter_destroy(bloomFilter);
    return SUCCESS;
}
