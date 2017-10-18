/*
 * threadpool.h
 *
 *  Created on: May 23, 2016
 *      Author: vangelis
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

typedef struct ThreadPool {
	int size;
	int *socketFileDescriptor;
	int start;
	int end;
	int count;
} ThreadPool;

void CreateThreadPool(ThreadPool *threadPool, int poolSize);

void DestroyThreadPool(ThreadPool *threadPool);

#endif /* THREADPOOL_H_ */
