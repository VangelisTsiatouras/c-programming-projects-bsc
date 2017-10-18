/*
 * hashtable.c

 *
 *  Created on: Mar 3, 2016
 *      Author: vangelis
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

HashTable* CreateHashtable(int numberOfBuckets) {
	int i, j, z;
	HashTable* hashTable = malloc(sizeof(HashTable));
	if (hashTable == NULL) {
		printf("MALLOC ERROR!\n");
		return NULL;
	}
	hashTable->size = numberOfBuckets;
	hashTable->storedNodes = 0;
	hashTable->nodeArray = malloc(sizeof(PtrToNodes) * numberOfBuckets);
	if (hashTable->nodeArray == NULL) {
		printf("MALLOC ERROR!\n");
		return NULL;
	}
	//Allocate space for 4 nodes per bucket
	for (i = 0; i < numberOfBuckets; i++) {
		//Initialization
		hashTable->nodeArray[i].sizeOfBucket = 4;
		hashTable->nodeArray[i].storedNodes = 0;
		hashTable->nodeArray[i].node = malloc(sizeof(Node) * 4);
		if (hashTable->nodeArray[i].node == NULL) {
			printf("MALLOC ERROR!\n");
			return NULL;
		}
		for (j = 0; j < hashTable->nodeArray[i].sizeOfBucket; j++) {
			hashTable->nodeArray[i].node[j].id = -1;
			hashTable->nodeArray[i].node[j].visited = 0;
			hashTable->nodeArray[i].node[j].pointerToIn = malloc(sizeof(Edge) * 4);
			if (hashTable->nodeArray[i].node[j].pointerToIn == NULL) {
				printf("MALLOC ERROR!\n");
				return NULL;
			}
			hashTable->nodeArray[i].node[j].pointerToOut = malloc(sizeof(Edge) * 4);
			if (hashTable->nodeArray[i].node[j].pointerToOut == NULL) {
				printf("MALLOC ERROR!\n");
				return NULL;
			}
			for (z = 0; z < 4; z++) {
				hashTable->nodeArray[i].node[j].pointerToIn[z].id = -1;
				hashTable->nodeArray[i].node[j].pointerToIn[z].transaction = 0;
				hashTable->nodeArray[i].node[j].pointerToOut[z].id = -1;
				hashTable->nodeArray[i].node[j].pointerToOut[z].transaction = 0;
			}
		}
	}
	return hashTable;
}

void DestroyHashtable(HashTable* hashTable) {
	int i, j;
	for (i = 0; i < hashTable->size; i++) {
		//Go to the buckets of the hashtable
		for (j = 0; j < hashTable->nodeArray[i].sizeOfBucket; j++) {
			//Go to the nodes
			if (hashTable->nodeArray[i].node[j].id != -1) {
				free(hashTable->nodeArray[i].node[j].pointerToOut);
				free(hashTable->nodeArray[i].node[j].pointerToIn);
			}
		}
		free(hashTable->nodeArray[i].node);
	}
	free(hashTable->nodeArray);
	free(hashTable);
	printf("hashtable successfully destroyed!\n");
}

int HashFunction(int key, int numberOfBuckets) {
	int position;
	position = key % numberOfBuckets;
	return position;
}
