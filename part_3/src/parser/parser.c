#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>

#include "../queryresults/queryresults.h"
#include "../ngramcounter/ngramcounter.h"
#include "../linearhash/linearhash.h"
#include "parser.h"
#include "../querylist/querylist.h"
#include "../queryresults/queryresults.h"
#include "../threadpool/threadpool.h"

void parser(Trie *trie, char *initFile, char *queryFile) {
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

    char *firstLine = NULL;
    size_t lineSize = 0;
    if (getline(&firstLine, &lineSize, iFile) > 0) {
        if (strncmp(firstLine, "DYNAMIC\n", 8) == 0) {
            dynamic_parser(trie, iFile, qFile);
        } else if (strncmp(firstLine, "STATIC\n", 7) == 0) {
            static_parser(trie, iFile, qFile);
        }
    }
    fclose(iFile);
    fclose(qFile);
    free(firstLine);
}

int dynamic_parser(Trie *trie, FILE *iFile, FILE *qFile) {
    BloomFilter *bloomFilter = create_bloom_filter();
    QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
    NgramCounter *ngramCounter = create_ngram_counter();
    char *line = NULL;
    char *word = NULL;
    int topk = 3;
    size_t lineSize = 0;
    char* saveptr;

    while (getline(&line, &lineSize, iFile) > 0) {
        if (check_whitespace(line)) {
            continue;
        }
        insert_trie(trie, line);
    }
    while (getline(&line, &lineSize, qFile) > 0) {
        NgramArray *ngramArray = NULL;
        switch (line[0]) {
            case 'Q':
                query_trie_dynamic(trie, &line[2], bloomFilter, queryResults, ngramCounter, 0);
//                copy_results_to_buffer_query_results_old(queryResults);
                break;
            case 'A':
                insert_trie(trie, &line[2]);
                break;
            case 'D':
                delete_ngram_trie(trie, &line[2]);

                break;
            case 'F':
//                pthread_mutex_lock(&mainThreadLock);
                word = strtok_r(line + 1, " \n", &saveptr);
                if (word != NULL) {
                    topk = atoi(word);
                    if (topk < 1) {
                        printf("Invalid top k given: %d\n", topk);
                        exit(WRONG_F_VALUE);
                    }
                    ngramArray = copy_to_ngram_array(ngramCounter);

                }
                print_query_results(queryResults);
                if (ngramArray != NULL) {
                    sort_topk(ngramArray, (unsigned int) topk);
                    destroy_ngram_array(ngramArray);
                }
                clear_ngram_counter(ngramCounter);
                break;
            default:
                printf("Unknown Command\n");
        }
    }
    destroy_gram_counter(ngramCounter);
    free(line);
    destroy_query_results(queryResults);
    destroy_bloom_filter(bloomFilter);
    return SUCCESS;
}

int static_parser(Trie *trie, FILE *iFile, FILE *qFile) {
    JobScheduler *jobScheduler = create_scheduler(NUMBER_OF_THREADS);
//    BloomFilter *bloomFilter = create_bloom_filter();
    BFStorage *bfStorage = create_bf_storage(NUMBER_OF_THREADS);
    QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
    NgramCounter *ngramCounter = create_ngram_counter();
    QueryList *queryList = create_querylist();
    char *line = NULL;
    char *word = NULL;
    int topk;
    size_t lineSize = 0;
    char *saveptr;

    while (getline(&line, &lineSize, iFile) > 0) {
        if (check_whitespace(line)) {
            continue;
        }
        insert_trie(trie, line);
    }
    free(line);
    compress_trie(trie);
    lineSize = 0;
    int queryID = 0;
    ListNode *iterator;
    while (getline(&line, &lineSize, qFile) > 0) {
        lineSize = 0;
        NgramArray *ngramArray = NULL;
        switch (line[0]) {
            case 'Q':
                insert_querylist(queryList, line + 2, queryID, 0);

//                query_trie_static(trie, &line[2], bfStorage, queryResults, ngramCounter, queryID);
                queryID++;
//                copy_results_to_buffer_query_results(queryResults);
//                print_query_results(queryResults);
                break;
            case 'F':
//                iterator = queryList->start;
//                while (iterator != NULL) {
//                    printf("%s", iterator->query);
//                    iterator = iterator->next;
//                }
                expand_query_results(queryResults, queryID);
                iterator = queryList->start;
                while (iterator != NULL) {
                    Job *job = create_job(7);
                    job->pointerToFunction = query_trie_static;
                    job->args[0] = trie;
                    job->args[1] = iterator->query;
                    job->args[2] = bfStorage;
                    job->args[3] = queryResults;
                    job->args[4] = ngramCounter;
//                    job->args[5] = malloc(sizeof(int));
//                    *(int *) job->args[5] = queryID;
                    job->args[5] = &iterator->query_ID;
                    job->args[6] = &queryList->elements;

                    submit_scheduler(jobScheduler, job);

                    iterator = iterator->next;
                }
//                printf("### MAIN THREAD: Waiting for workers...\n");
                pthread_mutex_lock(&mainThreadLock);
//                printf("number of queries %d qid %d\n", queryList->elements, queryID);

                while (queryResults->finished != queryList->elements) {
                    pthread_cond_wait(&mainThreadSleep, &mainThreadLock);

                }
                pthread_mutex_unlock(&mainThreadLock);
//                printf("### MAIN THREAD: Workers have finished\n");
//                pthread_mutex_lock(&finishedMutex);
//                pthread_mutex_unlock(&finishedMutex);

                empty_querylist(queryList);
                queryID = 0;
                word = strtok_r(line + 1, " \n", &saveptr);
                if (word != NULL) {
                    topk = atoi(word);
                    if (topk < 1) {
                        printf("Invalid top k given: %d\n", topk);
                        exit(WRONG_F_VALUE);
                    }
                    ngramArray = copy_to_ngram_array(ngramCounter);
                }
                print_query_results(queryResults);

                if (ngramArray != NULL) {
                    sort_topk(ngramArray, (unsigned int) topk);
                    destroy_ngram_array(ngramArray);
                }
                empty_querylist(queryList);
                clear_ngram_counter(ngramCounter);
                free(line);
//                pthread_mutex_lock(&mainThreadLock);
                break;
            default:
                free(line);
                printf("Unknown Command\n");
        }
    }
    free(line); //todo an vgei error mallon ftaei auto. einai ok gia ta arxeia eisodou
    destroy_gram_counter(ngramCounter);
    destroy_querylist(queryList);

    destroy_query_results(queryResults);
//    destroy_bloom_filter(bloomFilter);
    destroy_bf_storage(bfStorage);
    terminate_threads_scheduler(jobScheduler);
    destroy_scheduler(jobScheduler);
    return SUCCESS;
}

int check_whitespace(char *line) {
    while (*line != '\0') {
        if (!isspace((unsigned char) *line))
            return 0;
        line++;
    }
    return 1;
}
