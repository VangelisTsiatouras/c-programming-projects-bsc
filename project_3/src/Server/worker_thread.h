/*
 * worker_thread.h
 *
 *  Created on: May 23, 2016
 *      Author: vangelis
 */

#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

void *Worker(void* ptr);

void Connect(int socket);

void ExecuteCommand(char *command, int socket);

void AddAccountCommand(char **words, int numberOfWords, int socket);

void AddTransferCommand(char **words, int numberOfWords, int socket);

void AddMultiTransferCommand(char **words, int numberOfWords, int socket);

void PrintBalanceCommand(char **words, int numberOfWords, int socket);

void PrintMultiBalanceCommand(char **words, int numberOfWords, int socket);

void Sort(int *bucketArray, int size);

void MultiLock(int *sortedBuckets, int size);

void MultiUnlock(int *sortedBuckets, int size);

#endif /* WORKER_THREAD_H_ */
