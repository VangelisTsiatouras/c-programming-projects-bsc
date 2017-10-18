/*
 * hashtable.c
 *
 *  Created on: May 21, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "hashtable.h"

void CreateHashtable(HashTable *hashTable, int numberOfBuckets) {
	int i;
	/* Allocation */
	hashTable->size = numberOfBuckets;
	hashTable->buckets = malloc(sizeof(Bucket) * numberOfBuckets);
	if (hashTable->buckets == NULL) {
		printf("malloc error %s\n", strerror(errno));
		return;
	}
	for (i = 0; i < hashTable->size; i++) {
		if (pthread_mutex_init(&(hashTable->buckets[i].mtx), NULL) != 0) {
			printf("pthread_mutex_init %s\n", strerror(errno));
			return;
		}
	}
}

void DestroyHashtable(HashTable *hashTable) {
	int i;
	Account *nextAcc, *delAcc;
	for (i = 0; i < hashTable->size; i++) {
		nextAcc = hashTable->buckets[i].accounts;
		/* Destroy mutex */
		if (pthread_mutex_destroy(&(hashTable->buckets[i].mtx)) != 0) {
			printf("pthread_mutex_destroy %s\n", strerror(errno));
			return;
		}
		while (nextAcc != NULL) {
			delAcc = nextAcc;
			free(delAcc);
			nextAcc = nextAcc->nextAccount;
		}
		hashTable->buckets[i].accounts = NULL;
	}
	free(hashTable->buckets);
}

int HashFunction(char *key, int hashtableSize) {
	unsigned int hash = 0;
	int c;
	while ((c = *key++)) {
		hash += c;
	}
	return (hash % HASHTABLE_SIZE);
}

/* Return 0 when account is stored successfully
 * Return -1 for system errors
 * Return -2 if account is already stored
 */
int AddAccount(HashTable *hashTable, char *accName, int amount) {
	if (SearchAccount(hashTable, accName) > 0) {
		return -2;
	}
	int bucketNumber = HashFunction(accName, hashTable->size);
	/* Insert the new account */
	Account *newAcc = malloc(sizeof(Account));
	if (newAcc == NULL) {
		printf("malloc error %s\n", strerror(errno));
		return -1;
	}
	newAcc->name = malloc(sizeof(char) * NAMESIZE);
	strncpy(newAcc->name, accName, NAMESIZE);
	newAcc->amount = amount;
	newAcc->transfers = NULL;
	newAcc->nextAccount = hashTable->buckets[bucketNumber].accounts;
	hashTable->buckets[bucketNumber].accounts = newAcc;
	return 0;

}

/* Return 0 when transfer is completed
 * Return -1 for system errors
 * Return -2 when source account not exists
 * Return -3 when destination account not exists
 * Return -4 when there is not enough money in
 * 			 source account to complete the transfer
 */
int AddTransfer(HashTable *hashTable, char *srcName, char *dstName, int amount) {
	if (SearchAccount(hashTable, srcName) == 0) {
		return -2;
	}
	if (SearchAccount(hashTable, dstName) == 0) {
		return -3;
	}
	int found = 0;
	int srcBucketNumber = HashFunction(srcName, hashTable->size);
	int dstBucketNumber = HashFunction(dstName, hashTable->size);
	Account *tempSrcAcc = hashTable->buckets[srcBucketNumber].accounts;
	Account *tempDstAcc = hashTable->buckets[dstBucketNumber].accounts;
	/* Go to source account and subtract the amount */
	while (tempSrcAcc != NULL && found == 0) {
		if (strncmp(tempSrcAcc->name, srcName, 60) == 0) {
			if (tempSrcAcc->amount - amount > 0) {
				tempSrcAcc->amount -= amount;
				found = 1;
			}
			/* Not enough money in source account to complete the transfer */
			else {
				return -4;
			}
		}
		tempSrcAcc = tempSrcAcc->nextAccount;
	}
	/* Go to destination account and add the amount */
	found = 0;
	while (tempDstAcc != NULL && found == 0) {
		if (strncmp(tempDstAcc->name, dstName, 60) == 0) {
			tempDstAcc->amount += amount;
			found = 1;
		}
		tempDstAcc = tempDstAcc->nextAccount;
	}
	/* Add the transfer to the transfer list of the destination account */
	tempDstAcc = hashTable->buckets[dstBucketNumber].accounts;
	int transferFound = 0;
	Transfer *tempTransfer;
	Transfer *transfer = malloc(sizeof(Transfer));
	if (transfer == NULL) {
		printf("malloc error %s\n", strerror(errno));
		return -1;
	}
	transfer->name = malloc(sizeof(char) * NAMESIZE);
	if (transfer->name == NULL) {
		printf("malloc error %s\n", strerror(errno));
		return -1;
	}
	strncpy(transfer->name, srcName, NAMESIZE);
	transfer->transaction = amount;
	/* Search the transfer list if it has already stored a transfer from the same source account */
	tempTransfer = tempDstAcc->transfers;
	while (tempTransfer != NULL && transferFound == 0) {
		if (strncmp(tempTransfer->name, srcName, 60) == 0) {
			tempTransfer->transaction += amount;
			transferFound = 1;
		}
		tempTransfer = tempTransfer->nextTransfer;
	}
	/* If the transfer has not found in the transfer list push the new transfer to the list */
	if (transferFound == 0) {
		transfer->nextTransfer = tempDstAcc->transfers;
		tempDstAcc->transfers = transfer;
	}
	return 0;
}

/* Return the amount of money
 * Return -1 if the account was not found
 */
int PrintBalance(HashTable *hashTable, char *accName) {
	int amount = -1;
	int bucketNumber = HashFunction(accName, hashTable->size);
	Account *tempAccount = hashTable->buckets[bucketNumber].accounts;
	while (tempAccount != NULL && amount == -1) {
		if (strncmp(tempAccount->name, accName, 60) == 0) {
			amount = tempAccount->amount;
		}
		tempAccount = tempAccount->nextAccount;
	}
	return amount;
}

/* Return 1 if account found
 * Return 0 if account is not found
 */
int SearchAccount(HashTable *hashTable, char *accName) {
	int bucketNumber = HashFunction(accName, hashTable->size);
	int found = 0;
	Account *tempAcc = hashTable->buckets[bucketNumber].accounts;
	while (tempAcc != NULL) {
		if (strncmp(tempAcc->name, accName, 60) == 0) {
			found = 1;
			return found;
		}
		tempAcc = tempAcc->nextAccount;
	}
	return found;
}

void PrintHashtable(HashTable *hashTable) {
	int i;
	Account *tempAcc;
	Transfer *tempTran;
	for (i = 0; i < hashTable->size; i++) {
		tempAcc = hashTable->buckets[i].accounts;
		while (tempAcc != NULL) {
			printf("bucket: %d, account name: %s, amount: %d\n", i, tempAcc->name, tempAcc->amount);
			tempTran = tempAcc->transfers;
			while(tempTran != NULL){
				printf("	account name: %s, transaction: %d\n", tempTran->name, tempTran->transaction);
				tempTran = tempTran->nextTransfer;
			}
			tempAcc = tempAcc->nextAccount;
		}
	}
}
