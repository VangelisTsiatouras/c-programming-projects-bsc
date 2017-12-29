#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "queryresults.h"

QueryResults *create_query_results(int lines, size_t lineSize) {
    pthread_mutex_init(&finishedMutex, 0);
    pthread_mutex_init(&mainThreadLock, 0);
    pthread_cond_init(&mainThreadSleep, 0);
    QueryResults *queryResults = malloc(sizeof(QueryResults));
    if (!queryResults) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queryResults->elements = 0;
    queryResults->finished = 0;
    queryResults->totalLines = lines;
    queryResults->lineSize = malloc(lines * sizeof(size_t));
    if (!queryResults->lineSize) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queryResults->offsets = malloc(lines * sizeof(size_t));
    if (!queryResults->offsets) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queryResults->lines = malloc(lines * sizeof(char *));
    if (!queryResults->lines) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    for (int i = 0; i < lines; i++) {
        queryResults->lineSize[i] = lineSize;
        queryResults->offsets[i] = 0;
        queryResults->lines[i] = malloc(lineSize * sizeof(char));
        if (!queryResults->lines[i]) {
            printf("malloc error %s\n", strerror(errno));
            exit(MALLOC_ERROR);
        }
        queryResults->lines[i][0] = '\0';
    }
    return queryResults;
}

void destroy_query_results(QueryResults *queryResults) {
    for (int i = 0; i < queryResults->totalLines; i++) {
        free(queryResults->lines[i]);
    }
    pthread_mutex_destroy(&finishedMutex);
    pthread_mutex_destroy(&mainThreadLock);
    pthread_cond_destroy(&mainThreadSleep);
    free(queryResults->lines);
    free(queryResults->lineSize);
    free(queryResults->offsets);
    free(queryResults);
}

void expand_query_results(QueryResults *queryResults, int newSize) {
    if (newSize > queryResults->totalLines) {
        queryResults->lines = realloc(queryResults->lines, newSize * sizeof(char *));
        if (!queryResults->lines) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
        queryResults->lineSize = realloc(queryResults->lineSize, newSize * sizeof(size_t));
        if (!queryResults->lineSize) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
        queryResults->offsets = realloc(queryResults->offsets, newSize * sizeof(size_t));
        if (!queryResults->offsets) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
        for (int i = queryResults->totalLines; i < newSize; i++) {
            queryResults->lineSize[i] = DEFAULT_LINE_SIZE;
            queryResults->offsets[i] = 0;
            queryResults->lines[i] = malloc(DEFAULT_LINE_SIZE * sizeof(char));
            if (!queryResults->lines[i]) {
                printf("malloc error %s\n", strerror(errno));
                exit(MALLOC_ERROR);
            }
            queryResults->lines[i][0] = '\0';
        }
        queryResults->totalLines = newSize;
    }
}

void print_query_results(QueryResults *queryResults) {
    for (int i = 0; i < queryResults->elements; i++) {
        printf("%s\n", queryResults->lines[i]);
    }
    clear_query_results(queryResults);
}

int add_line_query_results_append(QueryResults *queryResults, char *newLine, int position) { //todo na ginei void
//    if (position == 0){//todo na fugei
//        printf("add line\n");
//    }
    size_t wordSize = strlen(newLine) + 2;
    size_t newLineSize = wordSize;
    size_t currentSize = queryResults->lineSize[position];
    size_t avaliableSize = currentSize - queryResults->offsets[position];
    // If new line cant fit in existing space
    if (newLineSize > avaliableSize) {
        if (newLineSize + queryResults->offsets[position] < currentSize * 2) {
            newLineSize = currentSize * 2;
        } else {
            newLineSize += currentSize;
        }
        queryResults->lineSize[position] = newLineSize;

        queryResults->lines[position] = (char *)realloc(queryResults->lines[position], newLineSize * sizeof(char));
        if (!queryResults->lines[position]) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
    }
    if (queryResults->lines[position][0] == '\0') {
        queryResults->elements++;
        strcpy(queryResults->lines[position], newLine);
    } else {
        strcat(queryResults->lines[position], "|");
        strcat(queryResults->lines[position], newLine);
    }
    queryResults->offsets[position] += wordSize;
    return SUCCESS;
}


void clear_query_results(QueryResults *queryResults) {
    for (int i = 0; i < queryResults->totalLines; i++) {
        queryResults->lines[i][0] = '\0';
        queryResults->offsets[i] = 0;
    }
    queryResults->elements = 0;
}

void wake_main_thread(QueryResults *queryResults, int totalQueries) {

    pthread_mutex_lock(&finishedMutex);
    queryResults->finished++;
//    printf("totalq %d fin %d\n", totalQueries, queryResults->finished);
    if (queryResults->finished == totalQueries) {
//        printf("time to really wake\n");
        pthread_cond_signal(&mainThreadSleep);
        //pthread_mutex_unlock(&mainThreadLock);
        // wake main thread
    }
    pthread_mutex_unlock(&finishedMutex);
}

void tester_query_results() {
    QueryResults *queryResults = create_query_results(5, DEFAULT_LINE_SIZE);

    char str[3] = "aa";
    for (int i = 0; i < 20000; i++) {
        printf("i: %d\n", i);
        int r = rand() % 26;
        int r2 = rand() % 26;
        str[0] += r;
        str[1] += r2;
        add_line_query_results_append(queryResults, str, rand() % 5);
        str[0] -= r;
        str[1] -= r2;
    }
    print_query_results(queryResults);
    expand_query_results(queryResults, 10);
    for (int i = 0; i < 20000; i++) {
        printf("i: %d\n", i);
        int r = rand() % 26;
        int r2 = rand() % 26;
        str[0] += r;
        str[1] += r2;
        add_line_query_results_append(queryResults, str, rand() % 10);
        str[0] -= r;
        str[1] -= r2;
    }
    print_query_results(queryResults);
    destroy_query_results(queryResults);
    exit(0);
}
