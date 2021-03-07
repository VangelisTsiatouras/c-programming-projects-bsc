#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

pthread_mutex_t queueMutex;
pthread_cond_t queueNotEmpty;

typedef struct Queue {
    unsigned int elements;
    struct QueueNode* head;
    struct QueueNode* tail;
} Queue;

typedef struct QueueNode {
    struct QueueNode* next;
    struct QueueNode* prev;
    struct Job* job;
} QueueNode;

typedef struct JobScheduler {
    int numberOfThreads;  // number of execution threads
    Queue* queue;         // a queue that holds submitted jobs / tasks
    pthread_t* threadIds; // execution threads
} JobScheduler;

typedef struct Job {
    void *pointerToFunction;
    void **args;
    int numberOfArgs;
} Job;

Queue* create_queue();
void destroy_queue(Queue* queue);
void push_queue(Queue* queue, void* job);
void* pop_queue(Queue* queue);

JobScheduler *create_scheduler(unsigned int numberOfThreads);
void destroy_scheduler(JobScheduler *jobScheduler);
void submit_scheduler(JobScheduler *jobScheduler, void *job);
void worker_scheduler(Queue *queue);
void terminate_threads_scheduler(JobScheduler *jobScheduler);
void function_caller_scheduler(Job *job);

Job *create_job(int numberOfArgs);
void destroy_job(Job *job);

void queue_tester();
void thread_tester();

void *tester_hello(int *x);
void *tester_hello2(int *x, int *y);

#endif // THREADPOOL_H
