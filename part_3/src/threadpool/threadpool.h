#ifndef THREADPOOL_H
#define THREADPOOL_H

typedef struct Queue {
    unsigned int elements;
    struct QueueNode* head;
    struct QueueNode* tail;
} Queue;

typedef struct QueueNode {
    struct QueueNode* next;
    struct QueueNode* prev;
    void* job;
} QueueNode;

Queue* create_queue();
void destroy_queue(Queue* queue);
void push_queue(Queue* queue, void* job);
void* pop_queue(Queue* queue);
void queue_tester();

#endif // THREADPOOL_H
