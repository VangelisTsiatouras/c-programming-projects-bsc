/*
 * hashtable.h
 *
 *  Created on: Oct 9, 2016
 *      Author: vangelis
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "record.h"

typedef struct CollisionList {
	Record *ptrToRecord;
	struct CollisionList *nextRecord;
}CollisionList;

typedef struct Bucket {
	CollisionList *headOfList;
} Bucket;

typedef struct HashTable {
	int size;
	int storedRecs;
	Bucket *buckets;
} HashTable;

/* FUNCTIONS */

int HashFunction(int key, int hashtableSize);

HashTable* CreateHT(int numberOfBuckets);

void DestroyHT(HashTable *hashTable);

int InsertRecordHT(HashTable *hashTable, Record *record);

int DeleteRecordHT(HashTable *hashTable, Record *record);

float AveragePostCodeHT(HashTable *hashTable, int postcode);

int KTOPAveragePostCodeHT(HashTable *hashTable, int postcode, int k);

int CoursesToTakeHT(HashTable *hashTable, int postcode, char *deprt);

float PercentilePostCodeHT(HashTable *hashTable, int postcode);

int PercentilesHT(HashTable *hashTable);

#endif /* HASHTABLE_H_ */
