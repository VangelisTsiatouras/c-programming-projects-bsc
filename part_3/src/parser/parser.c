#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>

#include "parser.h"
#include "../queryresults/queryresults.h"
#include "../ngramcounter/ngramcounter.h"
#include "../linearhash/linearhash.h"
#include "../querylist/querylist.h"
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
    JobScheduler *jobScheduler = create_scheduler(NUMBER_OF_THREADS);
    BFStorage *bfStorage = create_bf_storage(NUMBER_OF_THREADS);
    QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
    NgramCounter *ngramCounter = create_ngram_counter();
    QueryList *queryList = create_querylist();
    char *saveptr;
    char *line = NULL;
    char *word = NULL;
    int topk;
    size_t lineSize = 0;

    int versionID = 0;
    while (getline(&line, &lineSize, iFile) > 0) {
        if (check_whitespace(line)) {
            continue;
        }
        insert_ngram_version_trie(trie, line, 0);
    }
    free(line);
    lineSize = 0;
    int queryID = 0;
    ListNode *iterator;
    while (getline(&line, &lineSize, qFile) > 0) {
        lineSize = 0;
        NgramArray *ngramArray = NULL;
        switch (line[0]) {
            case 'Q':
                insert_querylist(queryList, line + 2, queryID, versionID);
                queryID++;
                break;
            case 'A':
                versionID++;
                insert_ngram_version_trie(trie, &line[2], versionID);
                free(line);
                break;
            case 'D':
                //TODO na klh8ei h physical delete sto F na adeiasei ligo h mnhmh giati gemizei 2GB sto large
                versionID++;
                delete_ngram_version_trie(trie, &line[2], versionID);
                free(line);
                break;
            case 'F':
                expand_query_results(queryResults, queryID);
                iterator = queryList->start;

                while (iterator != NULL) {
//                    query_trie_dynamic(trie, iterator->query, bloomFilter, queryResults, ngramCounter,
//                                       &iterator->query_ID, &queryList->elements, &iterator->version);
                    Job *job = create_job(8);
                    job->pointerToFunction = query_trie_dynamic;
                    job->args[0] = trie;
                    job->args[1] = iterator->query;
                    job->args[2] = bfStorage;
                    job->args[3] = queryResults;
                    job->args[4] = ngramCounter;
                    job->args[5] = &iterator->query_ID;
                    job->args[6] = &queryList->elements;
                    job->args[7] = &iterator->version;

                    submit_scheduler(jobScheduler, job);

                    iterator = iterator->next;
                }

                pthread_mutex_lock(&mainThreadLock);
//                printf("number of queries %d qid %d\n", queryList->elements, queryID);

                while (queryResults->finished != queryList->elements) {
                    pthread_cond_wait(&mainThreadSleep, &mainThreadLock);

                }
                pthread_mutex_unlock(&mainThreadLock);

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
                clear_ngram_counter(ngramCounter);
                free(line);
                break;
            default:
                printf("Unknown Command\n");
                free(line);
        }
    }
    free(line);
    destroy_bloom_filter(bloomFilter);
    destroy_gram_counter(ngramCounter);
    destroy_querylist(queryList);
    destroy_query_results(queryResults);
    destroy_bf_storage(bfStorage);
    terminate_threads_scheduler(jobScheduler);
    destroy_scheduler(jobScheduler);
    return SUCCESS;
}

int static_parser(Trie *trie, FILE *iFile, FILE *qFile) {
    JobScheduler *jobScheduler = create_scheduler(NUMBER_OF_THREADS);
    BFStorage *bfStorage = create_bf_storage(NUMBER_OF_THREADS);
    QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
    NgramCounter *ngramCounter = create_ngram_counter();
    QueryList *queryList = create_querylist();
    char *saveptr;
    char *line = NULL;
    char *word = NULL;
    int topk;
    size_t lineSize = 0;

    while (getline(&line, &lineSize, iFile) > 0) {
        if (check_whitespace(line)) {
            continue;
        }
        insert_ngram_trie(trie, line);
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
                queryID++;
                break;
            case 'F':
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
