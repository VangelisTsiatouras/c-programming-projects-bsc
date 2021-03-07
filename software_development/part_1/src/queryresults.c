#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "queryresults.h"

QueryResults *create_query_results(int lines, size_t lineSize) {
    QueryResults *queryResults = malloc(sizeof(QueryResults));
    if (!queryResults) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queryResults->elements = 0;
    queryResults->totalLines = lines;
    queryResults->lineSize = malloc(lines * sizeof(size_t));
    if (!queryResults->lineSize) {
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
        queryResults->lines[i] = malloc(lineSize * sizeof(char));
        if (!queryResults->lines[i]) {
            printf("malloc error %s\n", strerror(errno));
            exit(MALLOC_ERROR);
        }
    }
    queryResults->printBuffer = malloc(DEFAULT_PRINT_BUFFER * sizeof(char));
    if (!queryResults->printBuffer) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queryResults->printBufferSize = DEFAULT_PRINT_BUFFER;
    queryResults->printBufferOffset = 0;
    return queryResults;
}

void destroy_query_results(QueryResults *queryResults) {
    for (int i = 0; i < queryResults->totalLines; i++) {
        free(queryResults->lines[i]);
    }
    free(queryResults->lines);
    free(queryResults->lineSize);
    free(queryResults->printBuffer);
    free(queryResults);
}

void copy_results_to_buffer_query_results(QueryResults *queryResults) {
    char *buffer = queryResults->printBuffer;
    int offset = queryResults->printBufferOffset;
    for (int i = 0; i < queryResults->elements; i++) {
        size_t length = queryResults->lineSize[i] + 1;
        size_t newSize = offset + length + 2;
        if (newSize > queryResults->printBufferSize) {
            queryResults->printBufferSize *= 2;
            if (newSize > queryResults->printBufferSize) {
                queryResults->printBufferSize = newSize;
            }
            buffer = realloc(buffer, queryResults->printBufferSize * sizeof(char));
            if (!buffer) {
                printf("realloc error %s\n", strerror(errno));
                exit(REALLOC_ERROR);
            }
            queryResults->printBuffer = buffer;
        }
        if (i != 0) {
            offset += sprintf(buffer + offset, "|");
        }
        offset += sprintf(buffer + offset, "%s", queryResults->lines[i]);
    }
    offset += sprintf(buffer + offset, "\n");
    queryResults->printBufferOffset = offset;
    clear_query_results(queryResults);
}

void flush_query_results(QueryResults *queryResults) {
    printf("%s", queryResults->printBuffer);
    queryResults->printBufferOffset = 0;
    queryResults->printBuffer[0] = '\0';
    clear_query_results(queryResults);
}

int add_line_query_results(QueryResults *queryResults, char *newLine) {
    int position = queryResults->elements;
    for (int i = 0; i < position; i++) {
        if (strcmp(newLine, queryResults->lines[i]) == 0) {
            return ADD_LINE_FOUND;
        }
    }
    size_t newLineSize = strlen(newLine) + 1;
    // If there aren't any empty lines
    if (position == queryResults->totalLines) {
        queryResults->totalLines++;
        queryResults->lines = realloc(queryResults->lines, queryResults->totalLines * sizeof(char *));
        if (!queryResults->lines) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
        queryResults->lineSize = realloc(queryResults->lineSize, queryResults->totalLines * sizeof(size_t));
        if (!queryResults->lineSize) {
            printf("realloc error %s\n", strerror(errno));
            exit(REALLOC_ERROR);
        }
        queryResults->lineSize[position] = newLineSize;
        queryResults->lines[position] = malloc(newLineSize * sizeof(char));
        if (!queryResults->lines[position]) {
            printf("malloc error %s\n", strerror(errno));
            exit(MALLOC_ERROR);
        }
        strcpy(queryResults->lines[position], newLine);
        queryResults->elements++;
        queryResults->lineSize[position] = newLineSize;
        return SUCCESS;
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
            exit(REALLOC_ERROR);
        }
    }
    strcpy(queryResults->lines[position], newLine);
    queryResults->elements++;
    queryResults->lineSize[position] = newLineSize;
    return SUCCESS;
}

void clear_query_results(QueryResults *queryResults) {
    for (int i = 0; i < queryResults->totalLines; i++) {
        queryResults->lines[i][0] = '\0';
    }
    queryResults->elements = 0;
}
