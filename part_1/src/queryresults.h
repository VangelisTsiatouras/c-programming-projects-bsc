#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#define DEFAULT_LINES 8
#define DEFAULT_LINE_SIZE 64
#define DEFAULT_PRINT_BUFFER 1024

typedef struct QueryResults{
    char **lines;
    size_t *lineSize;
    int totalLines;
    int elements;
    char *printBuffer;
    size_t printBufferSize;
    int printBufferOffset;
} QueryResults;

//todo sunarthsh pou na kanei print se string kai na exei static metavlhth


QueryResults *createQueryResults(int lines, size_t lineSize);

void destroyQueryResults(QueryResults *queryResults);

void copyResultsToBufferQueryResults(QueryResults *queryResults);

void flushQueryResults(QueryResults *queryResults);

int addLineQueryResults(QueryResults *queryResults, char *line);

void clearQueryResults(QueryResults *queryResults);

void unitTestQueryResults();

#endif //QUERYRESULTS_H
