/*
 * main.c
 *
 *  Created on: May 21, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "hashtable.h"
#include "threadpool.h"
#include "master_thread.h"
#include "worker_thread.h"
#include "declare_globals.h"

HashTable hashTable;
ThreadPool threadPool;

pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

int main(int argc, char **argv) {
	int i, minusP = 0, minusS = 0, minusQ = 0, NOport = 0, NOthread_pool_size = 0, NOqueue_size = 0;
	int port, thread_pool_size, queue_size;
	/* Split args */
	if (argc > 7) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 7) {
		printf("too few arguments!\n");
		return 1;
	} else {
		for (i = 0; i < argc; ++i) {
			if (strncmp(argv[i], "-p", 2) == 0) {
				minusP = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-s", 2) != 0 && strncmp(argv[i + 1], "-q", 2) != 0) {
					port = atoi(argv[i + 1]);
				} else {
					NOport = 1;
				}
			} else if (strcmp(argv[i], "-s") == 0) {
				minusS = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-p", 2) != 0 && strncmp(argv[i + 1], "-q", 2) != 0) {
					thread_pool_size = atoi(argv[i + 1]);
				} else {
					NOthread_pool_size = 1;
				}
			} else if (strcmp(argv[i], "-q") == 0) {
				minusQ = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-p", 2) != 0 && strncmp(argv[i + 1], "-s", 2) != 0) {
					queue_size = atoi(argv[i + 1]);
				} else {
					NOqueue_size = 1;
				}
			}
		}
	}
	if (minusP == 0) {
		printf("flag -p is missing\ntype -p <port>\nserver exiting\n");
		return 1;
	}
	if (minusS == 0) {
		printf("flag -s is missing\ntype -s <thread_pool_size>\nserver exiting\n");
		return 1;
	}
	if (minusQ == 0) {
		printf("flag -p is missing\ntype -q <queue_size>\nserver exiting\n");
		return 1;
	}
	if (NOport == 1) {
		printf("number of port not entered\ntype -p <port>\nserver exiting\n");
		return 1;
	}
	if (NOthread_pool_size == 1) {
		printf("size of thread pool not entered\ntype -s <thread_pool_size>\nserver exiting\n");
		return 1;
	}
	if (NOqueue_size == 1) {
		printf("size of execution queue not entered\ntype -q <queue_size>\nserver exiting\n");
		return 1;
	}
	if (port <= 0 || thread_pool_size <= 0 || queue_size <= 0) {
		printf("the arguments must be greater than 0\nserver exiting\n");
		return 1;
	}
	printf("port: %d, numberOfThreads: %d, queuesize: %d\n", port, thread_pool_size, queue_size);
	/* Initialize hashtable */
	CreateHashtable(&hashTable, HASHTABLE_SIZE);
	/* Initialize thread pool */
	CreateThreadPool(&threadPool, queue_size);
	/* Initialize mutexes for threadpool */
	pthread_mutex_init(&mtx, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);
	/* Create worker threads */
	pthread_t threadIDs[thread_pool_size];
	for (i = 0; i < thread_pool_size; i++) {
		pthread_create(&threadIDs[i], NULL, Worker, NULL);
	}
	/* Call master thread */
	MasterThread(port, queue_size);
	/* Destroy worker threads */
	for (i = 0; i < thread_pool_size; i++) {
		pthread_join(threadIDs[i], 0);
	}
	/* Destroy mutexes */
	pthread_cond_destroy(&cond_nonempty);
	pthread_cond_destroy(&cond_nonfull);
	pthread_mutex_destroy(&mtx);
	return 0;
}
