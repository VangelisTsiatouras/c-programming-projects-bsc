#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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
    pthread_mutex_lock(&queueMutex);
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
    } else {
        temp->prev = queueNode;
        queue->elements++;
    }
    pthread_cond_signal(&queueNotEmpty);
    pthread_mutex_unlock(&queueMutex);
}

void *pop_queue(Queue* queue) {
    pthread_mutex_lock(&queueMutex);
    while (queue->elements == 0) {
        pthread_cond_wait(&queueNotEmpty, &queueMutex);
    }
    if (queue->elements == 0) {
        printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa"); //TODO na fugei olo to if ama paizei swsta
        return NULL;
    }
    QueueNode* temp = queue->head;
    void* returnValue = temp->job;
    if (queue->elements == 1) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->elements = 0;
        free(temp);
    } else {
        queue->head = temp->prev;
        temp->prev->next = NULL;
        queue->elements--;
        free(temp);
    }
    pthread_mutex_unlock(&queueMutex);
    return returnValue;
}

void queue_tester() {
    int reps = 100;
    Queue* queue = create_queue();
    for (int i = 0; i < reps; i++) {
        Job* p = malloc(sizeof(Job));
        p->args = malloc(sizeof(int));
        *(int*)p->args = i;
//        int *p2;
//        p2 = p->args;
//        *p2 = i;
        push_queue(queue, p);
    }
    for (int i = 0; i < reps + 2; i++) {
        Job *p = pop_queue(queue);
        if (p == NULL) {
            printf("null\n");
        } else {
            printf("%d\n", *(int*)p->args);
            free(p->args);
            free(p);
        }
    }
    destroy_queue(queue);
    exit(0);
}

JobScheduler *create_job_scheduler(unsigned int numberOfThreads) {
    JobScheduler *jobScheduler = malloc(sizeof(JobScheduler));
    if (!jobScheduler) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    jobScheduler->threadIds = malloc(sizeof(pthread_t) * numberOfThreads);
    if (!jobScheduler->threadIds) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    jobScheduler->queue = create_queue();
    jobScheduler->numberOfThreads = numberOfThreads;
    pthread_mutex_init(&queueMutex, 0);
    pthread_cond_init(&queueNotEmpty, 0);
    for (int i = 0; i < jobScheduler->numberOfThreads; i++) {
        pthread_create(&jobScheduler->threadIds[i], NULL, worker, jobScheduler->queue);
    }
    return jobScheduler;
}

void destroy_job_scheduler(JobScheduler *jobScheduler) {
    printf("Waiting for kids to exit\n");
    for (int i = 0; i < jobScheduler->numberOfThreads; i++) {
        pthread_join(jobScheduler->threadIds[i], 0);
    }
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueNotEmpty);
    destroy_queue(jobScheduler->queue);
    free(jobScheduler->threadIds);
    free(jobScheduler);
}

void submit_job_scheduler(JobScheduler *jobScheduler, void *job) {
    push_queue(jobScheduler->queue, job);
}

void worker(Queue *queue) {
    printf("I am thread\n");
    fflush(stdout);
    void * (*function) ();
    while(1) {
        Job *job = pop_queue(queue);
        if (job->args == NULL) {
            printf("exiting...\n");
            fflush(stdout);
            break;
        }
        function = job->pointerToFunction;
        function(*(int*)job->args);
    }
}

void tell_threads_to_exit(JobScheduler *jobScheduler) {
    printf("Telling kids to exit\n");
    for (int i = 0; i < jobScheduler->numberOfThreads; i++) {
        Job *nullJob = malloc(sizeof(Job));
        if (!nullJob) {
            printf("malloc error %s\n", strerror(errno));
            exit(MALLOC_ERROR);
        }
        nullJob->pointerToFunction = NULL;
        nullJob->args = NULL;
//        Job *nullJob = NULL;
        submit_job_scheduler(jobScheduler, nullJob);
//        push_queue(jobScheduler->queue, nullJob);
    }
}

void thread_tester() {
    JobScheduler *jobScheduler = create_job_scheduler(NUMBER_OF_THREADS);
    int reps = 10;
    for (int i = 0; i < reps; i++) {
        Job* p = malloc(sizeof(Job));
        p->pointerToFunction = hello;
        p->args = malloc(sizeof(int));
        *(int*)p->args = i;
//        int *p2;
//        p2 = p->args;
//        *p2 = i;
        submit_job_scheduler(jobScheduler, p);
    }
//    usleep(100);
    tell_threads_to_exit(jobScheduler);
    destroy_job_scheduler(jobScheduler);

    exit(0);
}

void *hello(int x){
    printf("hello %d\n", x);
    return NULL;
}