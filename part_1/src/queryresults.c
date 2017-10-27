#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "queryresults.h"

QueryResults *createQueryResults(int lines, size_t lineSize) {
    QueryResults *queryResults = malloc(sizeof(QueryResults));
    if (!queryResults) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    queryResults->elements = 0;
    queryResults->totalLines = lines;
    queryResults->lineSize = malloc(lines * sizeof(size_t));
    if (!queryResults->lineSize) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    queryResults->lines = malloc(lines * sizeof(char *));
    if (!queryResults->lines) {
        printf("malloc error %s\n", strerror(errno));
        exit(-1);
    }
    int i;
    for (i = 0; i < lines; i++) {
        queryResults->lineSize[i] = lineSize;
        queryResults->lines[i] = malloc(lineSize * sizeof(char));
        if (!queryResults->lines[i]) {
            printf("malloc error %s\n", strerror(errno));
            exit(-1);
        }
    }
    return queryResults;
}

void destroyQueryResults(QueryResults *queryResults) {
    int i;
    for (i = 0; i < queryResults->totalLines; i++) {
        free(queryResults->lines[i]);
    }
    free(queryResults->lines);
    free(queryResults->lineSize);
    free(queryResults);
}

void printQueryResults(QueryResults *queryResults) {
    int i;
    for (i = 0; i < queryResults->elements; ++i) {
        if (i != 0) {
            printf("|");
        }
        printf("%s", queryResults->lines[i]);
    }
    printf("\n");
}

int addLineQueryResults(QueryResults *queryResults, char *newLine) {
    int i;
    int position = queryResults->elements;

    for (i = 0; i < position; ++i) {
        if (strcmp(newLine, queryResults->lines[i]) == 0) {
            return 1;
        }
    }
    size_t newLineSize = strlen(newLine) + 1;
    // If there aren't any empty lines
    if (position == queryResults->totalLines) {
        queryResults->totalLines++;
        queryResults->lines = realloc(queryResults->lines, queryResults->totalLines * sizeof(char *));
        if (!queryResults->lines) {
            printf("realloc error %s\n", strerror(errno));
            exit(-1);
        }

        queryResults->lineSize = realloc(queryResults->lineSize, queryResults->totalLines * sizeof(size_t));
        if (!queryResults->lineSize) {
            printf("realloc error %s\n", strerror(errno));
            exit(-1);
        }
        queryResults->lineSize[position] = newLineSize;
        queryResults->lines[position] = malloc(newLineSize * sizeof(char));
        if (!queryResults->lines[position]) {
            printf("malloc error %s\n", strerror(errno));
            exit(-1);
        }
        strcpy(queryResults->lines[position], newLine);
        queryResults->elements++;
        queryResults->lineSize[position] = newLineSize;
        return 0;
    }
    size_t currentSize = queryResults->lineSize[position];
    // If new line cant fit in existing space
    if (newLineSize > currentSize) {
        if (newLineSize < currentSize * 2) {
            newLineSize = currentSize * 2;
        }
        queryResults->lines[position] = realloc(queryResults->lines[position], newLineSize * sizeof(char));
        if (!queryResults->lines[position]) {
            printf("realloc error %s\n", strerror(errno));
            exit(-1);
        }
    }
    strcpy(queryResults->lines[position], newLine);
    queryResults->elements++;
    queryResults->lineSize[position] = newLineSize;
    return 0;
}

void clearQueryResults(QueryResults *queryResults) {
    int i;
    for (i = 0; i < queryResults->totalLines; i++) {
        queryResults->lines[i][0] = '\0';
    }
    queryResults->elements = 0;
}

void unitTestQueryResults() {
    QueryResults *queryResults = createQueryResults(3, 20);
    addLineQueryResults(queryResults, "Test sentence");
    addLineQueryResults(queryResults, "Second sentence");
    // Add a big sentence
    addLineQueryResults(queryResults, "This is a big line. This is a big line. This is a big line");
    printQueryResults(queryResults);
    // Add a sentence when you dont have space
    addLineQueryResults(queryResults, "Last sentence");
    // Print
    printQueryResults(queryResults);
    // Clear
    clearQueryResults(queryResults);
    // Print the empty
    printQueryResults(queryResults);
    // Destroy
    destroyQueryResults(queryResults);
}
