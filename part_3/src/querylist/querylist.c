#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "querylist.h"
#include "../definitions.h"

QueryList* create_querylist() {
    QueryList* qlist = malloc(sizeof(QueryList));
    if (!qlist) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    qlist->elements = 0;
    qlist->start = NULL;
    return qlist;
}

void destroy_querylist(QueryList *qlist) {
    ListNode* iterator = qlist->start;
    while (iterator != NULL) {
        ListNode* temp = iterator;
        iterator = iterator->next;
        free(temp->query);
        free(temp);
    }
    free(qlist);
}

void empty_querylist(QueryList *qlist) {
    ListNode* iterator = qlist->start;
    qlist->elements = 0;
    while (iterator != NULL) {
        ListNode* temp = iterator;
        iterator = iterator->next;
        free(temp->query);
        free(temp);
    }
}

void insert_querylist(QueryList *qlist, char *query, int query_ID, int version) {

    ListNode* listNode = malloc(sizeof(ListNode));
    if (!listNode) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    listNode->query = query;
    listNode->query_ID = query_ID;
    listNode->version = version;
    listNode->next = qlist->start;
    qlist->start = listNode;
    qlist->elements++;
}

void tester_querylist() {
    QueryList *queryList = create_querylist();
    for (int i = 0; i < 20; i++) {
        insert_querylist(queryList, malloc(50), i, 0);
    }
    destroy_querylist(queryList);
    exit(0);
}
