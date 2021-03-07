/*
 * hashtable.h
 *
 *  Created on: May 21, 2016
 *      Author: vangelis
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#define NAMESIZE 60
#define HASHTABLE_SIZE 1009

/* Linked list of the accounts who transfered money */
typedef struct Transfer {
	char *name;
	int transaction;
	struct Transfer *nextTransfer;
} Transfer;

/* Linked list of the accounts who have the same key
 * in the hash table */
typedef struct Account {
	char *name;
	int amount;
	Transfer *transfers;
	struct Account *nextAccount;
} Account;

/* Contains the head of the linked list
 * and the mutex lock of the bucket */
typedef struct Bucket {
	Account *accounts;
	pthread_mutex_t mtx;
} Bucket;

/* This is primary data structure, contains the size
 * of the hash table and an array of buckets */
typedef struct HashTable {
	int size;
	Bucket *buckets;
} HashTable;

void CreateHashtable(HashTable *hashTable, int numberOfBuckets);

void DestroyHashtable(HashTable *hashTable);

int HashFunction(char *key, int hashtableSize);

int AddAccount(HashTable *hashTable, char *accName, int ammount);

int AddTransfer(HashTable *hashTable, char *srcName, char *dstName, int ammount);

int PrintBalance(HashTable *hashTable, char *accName);

int SearchAccount(HashTable *hashTable, char *accName);

void PrintHashtable(HashTable *hashTable);

#endif /* HASHTABLE_H_ */
