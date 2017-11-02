#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#include "defs.h"

#define DEFAULT_LINES 8
#define DEFAULT_LINE_SIZE 64
#define DEFAULT_PRINT_BUFFER 1024

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
void clear_query_results(QueryResults *queryResults);

#endif //QUERYRESULTS_H
