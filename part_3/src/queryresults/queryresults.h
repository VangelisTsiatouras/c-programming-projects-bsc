#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#include "../definitions.h"
#include <pthread.h>

pthread_mutex_t finishedMutex;
pthread_mutex_t mainThreadLock;
pthread_cond_t mainThreadSleep;

typedef struct QueryResults {
    char **lines;
    size_t *lineSize;
    size_t *offsets;
    int totalLines;
    int finished;
} QueryResults;


QueryResults *create_query_results(int lines, size_t lineSize);
void destroy_query_results(QueryResults *queryResults);
void expand_query_results(QueryResults *queryResults, int newSize);
void print_query_results(QueryResults *queryResults);
int add_line_query_results(QueryResults *queryResults, char *newLine, int position);
void clear_query_results(QueryResults *queryResults);
void wake_main_thread(QueryResults *queryResults, int totalQueries);

void tester_query_results();

#endif //QUERYRESULTS_H
