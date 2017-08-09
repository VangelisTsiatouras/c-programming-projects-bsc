/*
 * worker_thread.c
 *
 *  Created on: May 23, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "worker_thread.h"
#include "hashtable.h"
#include "threadpool.h"
#include "declare_globals.h"

#define BUFFERSIZE 2048

void *Worker(void* ptr) {
	int socket;
	while (1) {
		/* Obtain a socket file descriptor from thread pool */
		pthread_mutex_lock(&mtx);
		while (threadPool.count <= 0) {
			printf(" >> Found Buffer Empty \n ");
			pthread_cond_wait(&cond_nonempty, &mtx);
		}
		socket = threadPool.socketFileDescriptor[threadPool.start];
		threadPool.start = (threadPool.start + 1) % threadPool.size;
		threadPool.count--;
		pthread_cond_signal(&cond_nonfull);
		pthread_mutex_unlock(&mtx);
		Connect(socket);
	}
	pthread_exit(0);
}

void Connect(int socket) {
	char buf[BUFFERSIZE];
	while ((read(socket, buf, BUFFERSIZE) > 0) && (strncmp(buf, "CLOSE", BUFFERSIZE) != 0)) {
		ExecuteCommand(buf, socket);
		if (write(socket, "END_OF_PRINT", BUFFERSIZE) < 0) {
			printf("write error %s\n", strerror(errno));
			return;
		}
		//PrintHashtable(&hashTable);
	}
	close(socket);
}

void ExecuteCommand(char *command, int socket) {
	/* Manipulate the received message */
	/* Split the message into separate words */
	char* token;
	char** words = NULL;
	char *saveptr;
	int numberOfSpaces = 0, tokencounter = 0;
	/* Split the entered line to words */
	token = strtok_r(command, " \t\n", &saveptr);
	while (token != NULL) {
		numberOfSpaces++;
		words = realloc(words, sizeof(char*) * numberOfSpaces);
		if (words == NULL) {
			printf("malloc error %s\n", strerror(errno));
			return;
		}
		words[tokencounter] = token;
		tokencounter++;
		token = strtok_r(NULL, " \t\n", &saveptr);
	}
	/* Execute given command */
	if (strcmp(words[0], "add_account") == 0) {
		AddAccountCommand(words, tokencounter, socket);
	} else if (strcmp(words[0], "add_transfer") == 0) {
		AddTransferCommand(words, tokencounter, socket);
	} else if (strcmp(words[0], "add_multi_transfer") == 0) {
		AddMultiTransferCommand(words, tokencounter, socket);
	} else if (strcmp(words[0], "print_balance") == 0) {
		PrintBalanceCommand(words, tokencounter, socket);
	} else if (strcmp(words[0], "print_multi_balance") == 0) {
		PrintMultiBalanceCommand(words, tokencounter, socket);
	}
	free(token);
	free(words);
}

void AddAccountCommand(char **words, int numberOfWords, int socket) {
	int delay = 0;
	if (numberOfWords == 4) {
		delay = atoi(words[3]);
	}
	int amount = atoi(words[1]);
	char *accName = words[2];
	char feedback[BUFFERSIZE];
	/* Lock the bucket */
	int bucketNumber = HashFunction(accName, HASHTABLE_SIZE);
	pthread_mutex_lock(&hashTable.buckets[bucketNumber].mtx);
	/* Delay */
	usleep(1000 * delay);
	/* Call hashtable's AddAccount function */
	int retCode = AddAccount(&hashTable, accName, amount);
	/* Give feedback to client */
	if (retCode == 0) {
		snprintf(feedback, BUFFERSIZE, "SUCCESS: Account with name %s and amount %d added\n", accName, amount);
	} else if (retCode == -1) {
		/* THIS SHOULD NEVER AND EVER BE HAPPEN */
		snprintf(feedback, BUFFERSIZE, "FAILURE: Fatal server error\n");
	} else if (retCode == -2) {
		snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s is already stored\n", accName);
	}
	if (write(socket, feedback, BUFFERSIZE) < 0) {
		printf("write error %s\n", strerror(errno));
		return;
	}
	/* Unlock the bucket */
	pthread_mutex_unlock(&hashTable.buckets[bucketNumber].mtx);
}

void AddTransferCommand(char **words, int numberOfWords, int socket) {
	int amount = atoi(words[1]);
	int delay = 0;
	char feedback[BUFFERSIZE];
	fflush(stdout);
	/* Check if the last word is delay
	 * If the first char is number then it is delay*/
	if (isdigit(words[numberOfWords - 1][0])) {
		delay = atoi(words[numberOfWords - 1]);
	}
	/* The maximum number of buckets to lock and unlock should be equal
	 * to the number of accounts which are involved in this transfer */
	int bucketsToLockUnlock = numberOfWords - 2;
	/* Don't count the delay as an account */
	if (delay != 0) {
		bucketsToLockUnlock--;
	}
	int i, wordIter = 2;
	/* Init array */
	int sortedBuckets[bucketsToLockUnlock];
	for (i = 0; i < bucketsToLockUnlock; i++) {
		sortedBuckets[i] = HashFunction(words[wordIter], HASHTABLE_SIZE);
		wordIter++;
	}
	/* Sort the buckets to lock them with order */
	Sort(sortedBuckets, bucketsToLockUnlock);
	/* Call MultiLock to lock the mutexes of each bucket which contain the accounts */
	MultiLock(sortedBuckets, bucketsToLockUnlock);
	/* Delay */
	usleep(1000 * delay);
	/* Call hashtable's AddTransfer function */
	int retCode = AddTransfer(&hashTable, words[2], words[3], amount);
	/* Give feedback to client */
	if (retCode == 0) {
		snprintf(feedback, BUFFERSIZE, "SUCCESS: Transfer from %s to %s with amount: %d completed successfully\n",
				words[2], words[3], amount);
	} else if (retCode == -1) {
		/* THIS SHOULD NEVER AND EVER BE HAPPEN */
		snprintf(feedback, BUFFERSIZE, "FAILURE: Fatal server error\n");
	} else if (retCode == -2) {
		snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s does not exist\n", words[2]);
	} else if (retCode == -3) {
		snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s does not exist\n", words[3]);
	} else if (retCode == -4) {
		snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s has not enough money to complete the transfer\n",
				words[2]);
	}
	if (write(socket, feedback, BUFFERSIZE) < 0) {
		printf("write error %s\n", strerror(errno));
		return;
	}
	/* Call MultiUnlock to unlock the mutexes of each bucket which contain the accounts */
	MultiUnlock(sortedBuckets, bucketsToLockUnlock);
}

void AddMultiTransferCommand(char **words, int numberOfWords, int socket) {
	int amount = atoi(words[1]);
	int delay = 0;
	char feedback[BUFFERSIZE];
	/* Check if the last word is delay
	 * If the first char is number then it is delay*/
	if (isdigit(words[numberOfWords - 1][0])) {
		delay = atoi(words[numberOfWords - 1]);
	}
	/* The number of buckets to lock and unlock at maximum should be equal
	 * to the number of accounts that are involved in this transfer */
	int bucketsToLockUnlock = numberOfWords - 2;
	/* Don't count the delay as an account */
	if (delay != 0) {
		bucketsToLockUnlock--;
	}
	/* bucketsToLockUnlock is equal to the number of accounts that are involved containing
	 * and the source account */
	int numberOfDestAccounts = bucketsToLockUnlock - 1;
	int i, wordIter = 2;
	/* Init array */
	int sortedBuckets[bucketsToLockUnlock];
	for (i = 0; i < bucketsToLockUnlock; i++) {
		sortedBuckets[i] = HashFunction(words[wordIter], HASHTABLE_SIZE);
		wordIter++;
	}
	Sort(sortedBuckets, bucketsToLockUnlock);
	/* Call MultiLock to lock the mutexes of each bucket which contain the accounts */
	MultiLock(sortedBuckets, bucketsToLockUnlock);
	/* Delay */
	usleep(1000 * delay);
	for (i = 0; i < numberOfDestAccounts; i++) {
		/* Call hashtable's AddTransfer function */
		int retCode = AddTransfer(&hashTable, words[2], words[3 + i], amount);
		/* Give feedback to client */
		if (retCode == 0) {
			snprintf(feedback, BUFFERSIZE, "SUCCESS: Transfer from %s to %s with amount: %d completed successfully\n",
					words[2], words[3 + i], amount);
		} else if (retCode == -1) {
			/* THIS SHOULD NEVER AND EVER BE HAPPEN */
			snprintf(feedback, BUFFERSIZE, "FAILURE: Fatal server error\n");
		} else if (retCode == -2) {
			snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s does not exist\n", words[2]);
		} else if (retCode == -3) {
			snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s does not exist\n", words[3 + i]);
		} else if (retCode == -4) {
			snprintf(feedback, BUFFERSIZE,
					"FAILURE: Account with name %s has not enough money to complete the transfer\n", words[2]);
		}
		if (write(socket, feedback, BUFFERSIZE) < 0) {
			printf("write error %s\n", strerror(errno));
			return;
		}
		usleep(50000);
	}
	/* Call MultiUnlock to unlock the mutexes of each bucket which contain the accounts */
	MultiUnlock(sortedBuckets, bucketsToLockUnlock);
}

void PrintBalanceCommand(char **words, int numberOfWords, int socket) {
	int delay = 0;
	char feedback[BUFFERSIZE];
	char *accName = words[1];
	/* Check if the last word is delay
	 * If the first char is number then it is delay*/
	if (isdigit(words[numberOfWords - 1][0])) {
		delay = atoi(words[numberOfWords - 1]);
	}
	/* Lock the bucket */
	int bucketNumber = HashFunction(accName, HASHTABLE_SIZE);
	pthread_mutex_lock(&hashTable.buckets[bucketNumber].mtx);
	/* Delay */
	usleep(1000 * delay);
	/* Call hashtable's PrintBalance function */
	int retCode = PrintBalance(&hashTable, accName);
	/* Give feedback to client */
	if (retCode >= 0) {
		snprintf(feedback, BUFFERSIZE, "SUCCESS: Balance of account with name %s : %d\n", words[1], retCode);
	} else {
		snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s does not exist\n", words[1]);
	}
	if (write(socket, feedback, BUFFERSIZE) < 0) {
		printf("write error %s\n", strerror(errno));
		return;
	}
	/* Unlock the bucket */
	pthread_mutex_unlock(&hashTable.buckets[bucketNumber].mtx);
}

void PrintMultiBalanceCommand(char **words, int numberOfWords, int socket) {
	int delay = 0;
	char feedback[BUFFERSIZE];
	/* Check if the last word is delay
	 * If the first char is number then it is delay*/
	if (isdigit(words[numberOfWords - 1][0])) {
		delay = atoi(words[numberOfWords - 1]);
	}
	/* The number of buckets to lock and unlock at maximum should be equal
	 * to the number of accounts that are involved */
	int bucketsToLockUnlock = numberOfWords - 1;
	/* Don't count the delay as an account */
	if (delay != 0) {
		bucketsToLockUnlock--;
	}
	int i, wordIter = 1;
	/* Init array */
	int sortedBuckets[bucketsToLockUnlock];
	for (i = 0; i < bucketsToLockUnlock; i++) {
		sortedBuckets[i] = HashFunction(words[wordIter], HASHTABLE_SIZE);
		wordIter++;
	}
	/* Sort the buckets to lock them with order */
	Sort(sortedBuckets, bucketsToLockUnlock);
	/* Call MultiLock to lock the mutexes of each bucket which contain the accounts */
	MultiLock(sortedBuckets, bucketsToLockUnlock);
	/* Delay */
	usleep(1000 * delay);
	for (i = 0; i < bucketsToLockUnlock; i++) {
		/* Call hashtable's PrintBalance function */
		int retCode = PrintBalance(&hashTable, words[1 + i]);
		/* Give feedback to client */
		if (retCode >= 0) {
			snprintf(feedback, BUFFERSIZE, "SUCCESS: Balance of account with name %s : %d\n", words[1 + i], retCode);
		} else {
			snprintf(feedback, BUFFERSIZE, "FAILURE: Account with name %s does not exist\n", words[1 + i]);
		}
		if (write(socket, feedback, BUFFERSIZE) < 0) {
			printf("write error %s\n", strerror(errno));
			return;
		}
		usleep(50000);
	}
	/* Call MultiUnlock to unlock the mutexes of each bucket which contain the accounts */
	MultiUnlock(sortedBuckets, bucketsToLockUnlock);
}

void Sort(int *bucketArray, int size) {
	int i, j, swap;
	/* Sort the array with ascending order. Also duplicate values set to -1,
	 * there is no bucket with id == -1 it will be ignored */
	for (i = 0; i < (size - 1); i++) {
		for (j = 0; j < (size - i - 1); j++) {
			if (bucketArray[j] > bucketArray[j + 1]) {
				swap = bucketArray[j];
				bucketArray[j] = bucketArray[j + 1];
				bucketArray[j + 1] = swap;
			}
			if (bucketArray[j] == bucketArray[j + 1]) {
				bucketArray[j] = -1;
			}
		}
	}
}

void MultiLock(int *bucketArray, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (bucketArray[i] != -1) {
			pthread_mutex_lock(&hashTable.buckets[bucketArray[i]].mtx);
		}
	}
}

void MultiUnlock(int *bucketArray, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (bucketArray[i] != -1) {
			pthread_mutex_unlock(&hashTable.buckets[bucketArray[i]].mtx);
		}
	}
}
