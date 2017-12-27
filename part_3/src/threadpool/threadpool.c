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
    return queue;
}

void destroy_queue(Queue* queue) {
    QueueNode* iterator = queue->head;
    while (iterator != NULL) {
        QueueNode* temp = iterator;
        iterator = iterator->next; // todo mallon einai fixed
        free(temp);

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

JobScheduler *create_scheduler(unsigned int numberOfThreads) {
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
        pthread_create(&jobScheduler->threadIds[i], NULL, (void *) worker_scheduler, jobScheduler->queue);
    }
    return jobScheduler;
}

void destroy_scheduler(JobScheduler *jobScheduler) {
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

void submit_scheduler(JobScheduler *jobScheduler, void *job) {
    push_queue(jobScheduler->queue, job);
}

void worker_scheduler(Queue *queue) {
    printf("I am thread\n");
    fflush(stdout);
    while(1) {
        Job *job = pop_queue(queue);
        printf("got job\n");
        fflush(stdout);
        if (job == NULL) {
            printf("exiting...\n");
            fflush(stdout);
            break;
        }
        function_caller_scheduler(job);
    }
}

void terminate_threads_scheduler(JobScheduler *jobScheduler) {
    printf("Telling kids to exit\n");

    for (int i = 0; i < jobScheduler->numberOfThreads; i++) {
        submit_scheduler(jobScheduler, NULL);
    }
}

void function_caller_scheduler(Job *job) {
    void * (*function) ()= job->pointerToFunction;
    switch (job->numberOfArgs) {
        case 0:
            function();
            break;
        case 1:
            function(job->args[0]);
            break;
        case 2:
            function(job->args[0], job->args[1]);
            break;
        case 3:
            function(job->args[0], job->args[1], job->args[2]);
            break;
        case 4:
            function(job->args[0], job->args[1], job->args[2], job->args[3]);
            break;
        case 5:
            function(job->args[0], job->args[1], job->args[2], job->args[3], job->args[4]);
            break;
        case 6:
            function(job->args[0], job->args[1], job->args[2], job->args[3], job->args[4], job->args[5]);
            break;
        case 7:
            function(job->args[0], job->args[1], job->args[2], job->args[3], job->args[4], job->args[5], job->args[6]);
            break;
        case 8:
            function(job->args[0], job->args[1], job->args[2], job->args[3], job->args[4], job->args[5], job->args[6], job->args[7]);
            break;
        case 9:
            function(job->args[0], job->args[1], job->args[2], job->args[3], job->args[4], job->args[5], job->args[6], job->args[7], job->args[8]);
            break;
        case 10:
            function(job->args[0], job->args[1], job->args[2], job->args[3], job->args[4], job->args[5], job->args[6], job->args[7], job->args[8], job->args[9]);
            break;
    }
    destroy_job(job);
}

Job *create_job(int numberOfArgs) {
    Job *job = malloc(sizeof(Job));
    if (!job) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    job->args = malloc(sizeof(void*) * numberOfArgs);
    if (!job->args) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    job->numberOfArgs = numberOfArgs;
    return job;
}

void destroy_job(Job *job) {
    free(job->args);
    free(job);
}

void queue_tester() {
    int reps = 100;
    Queue* queue = create_queue();
    for (int i = 0; i < reps; i++) {
        Job* p = malloc(sizeof(Job));
        p->args = malloc(sizeof(int));
        *(int*)p->args = i;
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

void thread_tester() {
    JobScheduler *jobScheduler = create_scheduler(NUMBER_OF_THREADS);
    int reps = 10;
    for (int i = 0; i < reps; i++) {
        Job *job = create_job(1);
        job->pointerToFunction = hello;
        job->args[0] = malloc(sizeof(int));
        *(int*)job->args[0] = i;
        submit_scheduler(jobScheduler, job);
    }
    for (int i = 0; i < reps; i++) {
        Job *job = create_job(2);
        job->pointerToFunction = hello2;
        job->args[0] = malloc(sizeof(int));
        job->args[1] = malloc(sizeof(int));
        *(int*)job->args[0] = i;
        *(int*)job->args[1] = i * i;
        submit_scheduler(jobScheduler, job);
    }
    terminate_threads_scheduler(jobScheduler);
    destroy_scheduler(jobScheduler);
    exit(0);
}

void *hello(int *x){
    printf("hello %d\n", *x);
    return NULL;
}

void *hello2(int *x, int *y){
    printf("hello2 %d %d\n", *x, *y);
    return NULL;
}

