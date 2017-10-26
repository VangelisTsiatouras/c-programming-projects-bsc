#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

typedef struct QueryResults{
    char **lines;
    size_t *lineSize;
    int totalLines;
    int elements;
} QueryResults;


QueryResults *createQueryResults(int lines, size_t lineSize);

void destroyQueryResults(QueryResults *queryResults);

void printQueryResults(QueryResults *queryResults);

int addLineQueryResults(QueryResults *queryResults, char *line);

void clearQueryResults(QueryResults *queryResults);

void unitTestQueryResults();

#endif //QUERYRESULTS_H
