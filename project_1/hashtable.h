/*
 * hashtable.h
 *
 *  Created on: Mar 3, 2016
 *      Author: vangelis
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "pointer_to_nodes.h"

typedef struct HashTable{
	int size;
	int storedNodes;
	PtrToNodes* nodeArray;
}HashTable;

HashTable* CreateHashtable(int numberOfBuckets);

void DestroyHashtable(HashTable* hashTable);

int HashFunction(int key, int numberOfBuckets);

#endif /* HASHTABLE_H_ */
