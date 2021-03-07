#ifndef QUERYLIST_H
#define QUERYLIST_H

typedef struct QueryList {
    unsigned int elements;
    struct ListNode* start;
} QueryList;

typedef struct ListNode {
    struct ListNode* next;
    char* query;
    int query_ID;
    int version;
} ListNode;

QueryList* create_querylist();
void destroy_querylist(QueryList *qlist);
void empty_querylist(QueryList *qlist);
void insert_querylist(QueryList *qlist, char *query, int query_ID, int version);

void tester_querylist();

#endif //QUERYLIST_H
