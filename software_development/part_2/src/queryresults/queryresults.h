#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#include "../definitions.h"

typedef struct QueryResults {
    char **lines;
    size_t *lineSize;
    int totalLines;
    int elements;
    char *printBuffer;
    size_t printBufferSize;
    int printBufferOffset;
} QueryResults;


QueryResults *create_query_results(int lines, size_t lineSize);
void destroy_query_results(QueryResults *queryResults);
void copy_results_to_buffer_query_results(QueryResults *queryResults);
void flush_query_results(QueryResults *queryResults);
int add_line_query_results(QueryResults *queryResults, char *line);
int add_line_query_results_append(QueryResults *queryResults, char *newLine);
void clear_query_results(QueryResults *queryResults);

#endif //QUERYRESULTS_H
