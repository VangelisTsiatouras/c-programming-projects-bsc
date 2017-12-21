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

Queue* create_queue();
void destroy_queue(Queue* queue);
void push_queue(Queue* queue, void* job);
void* pop_queue(Queue* queue);
void queue_tester();


typedef struct JobScheduler {
    int numberOfThreads;       // number of execution threads
    Queue* queue;              // a queue that holds submitted jobs / tasks
    pthread_t* threadIds;      // execution threads
// mutex, condition variable, ...
} JobScheduler;

typedef struct Job {
    void *pointerToFunction;
    void *args;
} Job;

JobScheduler *create_job_scheduler(unsigned int numberOfThreads);
void destroy_job_scheduler(JobScheduler *jobScheduler);
void submit_job_scheduler(JobScheduler *jobScheduler, void *job);
void worker(Queue *queue);

void thread_tester();

void *hello(int x);

#endif // THREADPOOL_H
