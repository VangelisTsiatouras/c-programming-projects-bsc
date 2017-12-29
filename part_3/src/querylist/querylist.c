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
        free(temp->query - 2);
        free(temp);
    }
    qlist->start = NULL;
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
//    for (int i = 0; i < 20; i++) {
//        insert_querylist(queryList, malloc(50), i, 0);
//    }
//

    for (int i = 0; i < 5; i++) {
        char *str = malloc(sizeof(char)*3);
        strcpy(str,"aa");
        int r = rand() % 26;
        int r2 = rand() % 26;
        str[0] += r;
        str[1] += r2;
        insert_querylist(queryList, str, i, 0);
    }
    ListNode *iterator = queryList->start;
    printf("1st insertion\n");
    while (iterator != NULL) {
        printf("%s\n", iterator->query);
        iterator = iterator->next;
    }

    empty_querylist(queryList);
    iterator = queryList->start;
    printf("empty\n");
    while (iterator != NULL) {
        printf("%s\n", iterator->query);
        iterator = iterator->next;
    }


    for (int i = 0; i < 5; i++) {
        char *str = malloc(sizeof(char)*3);
        strcpy(str,"aa");
        int r = rand() % 26;
        int r2 = rand() % 26;
        str[0] += r;
        str[1] += r2;
        insert_querylist(queryList, str, i, 0);
    }
    printf("2nd insertion\n");
    iterator = queryList->start;
    while (iterator != NULL) {
        printf("%s\n", iterator->query);
        iterator = iterator->next;
    }

    destroy_querylist(queryList);
    exit(0);
}
