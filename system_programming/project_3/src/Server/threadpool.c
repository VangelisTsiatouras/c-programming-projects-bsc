/*
 * threadpool.c
 *
 *  Created on: May 23, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "threadpool.h"

void CreateThreadPool(ThreadPool *threadPool, int poolSize) {
	/* Allocation */
	threadPool->size = poolSize;
	threadPool->socketFileDescriptor = malloc(sizeof(int) * poolSize);
	if (threadPool->socketFileDescriptor == NULL) {
		printf("malloc error %s\n", strerror(errno));
		return;
	}
	threadPool->start = 0;
	threadPool->end = -1;
	threadPool->count = 0;
}

void DestroyThreadPool(ThreadPool *threadPool) {
	free(threadPool->socketFileDescriptor);
}
