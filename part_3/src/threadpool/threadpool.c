#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "threadpool.h"
#include "../definitions.h"

Queue* create_queue() {
    Queue* queue = malloc(sizeof(Queue));
    if (!queue) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queue->elements = 0;
    queue->head = NULL;
    queue->tail = NULL;
}

void destroy_queue(Queue* queue) {
    QueueNode* iterator = queue->head;
    while (iterator != NULL) {
        QueueNode* temp = iterator;
        free(temp);
        iterator = iterator->next;
    }
    free(queue);
}

void push_queue(Queue* queue, void* job) {
    QueueNode* queueNode = malloc(sizeof(QueueNode));
    if (!queueNode) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    queueNode->job = job;
    queueNode->prev = NULL;
    queueNode->next = queue->tail;
    QueueNode* temp = queue->tail;
    queue->tail = queueNode;
    if (queue->elements == 0) {
        queue->head = queueNode;
        queue->elements++;
        return;
    }
    temp->prev = queueNode;
    queue->elements++;
}

void *pop_queue(Queue* queue) {
    if (queue->elements == 0) {
        return NULL;
    }
    QueueNode* temp = queue->head;
    void* returnValue = temp->job;
    if (queue->elements == 1) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->elements = 0;
        free(temp);
        return returnValue;
    }
    queue->head = temp->prev;
    temp->prev->next = NULL;
    queue->elements--;
    free(temp);
    return returnValue;
}

void queue_tester() {
    int reps = 100;
    Queue* queue = create_queue();
    for (int i = 0; i < reps; i++) {
        int* p = malloc(sizeof(int));
        *p = i;
        push_queue(queue, p);
    }
    for (int i = 0; i < reps + 2; i++) {
        int *p = pop_queue(queue);
        if (p == NULL) {
            printf("null\n");
        } else {
            printf("%d\n", *p);
            free(p);
        }
    }
    destroy_queue(queue);
    exit(0);
}
