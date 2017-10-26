#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#define DEFAULT_LINES 8
#define DEFAULT_LINE_SIZE 64

typedef struct QueryResults{
    char **lines;
    size_t *lineSize;
    int totalLines;
    int elements;
} QueryResults;

//todo sunarthsh pou na kanei print se string kai na exei static metavlhth


QueryResults *createQueryResults(int lines, size_t lineSize);

void destroyQueryResults(QueryResults *queryResults);

void printQueryResults(QueryResults *queryResults);

int addLineQueryResults(QueryResults *queryResults, char *line);

void clearQueryResults(QueryResults *queryResults);

void unitTestQueryResults();

#endif //QUERYRESULTS_H
