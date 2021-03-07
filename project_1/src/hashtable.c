/*
 * hashtable.c
 *
 *  Created on: Oct 9, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "hashtable.h"

#define BUFFERSIZE 256

int HashFunction(int key, int hashtableSize) {
	/*unsigned int hash(unsigned int x) {
	 x = ((x >> 16) ^ x) * 0x45d9f3b;
	 x = ((x >> 16) ^ x) * 0x45d9f3b;
	 x = (x >> 16) ^ x;
	 return x;
	 }*/
	int position;
	position = key % hashtableSize;
	return position;
}

HashTable* CreateHT(int numberOfBuckets) {
	HashTable *hashTable = malloc(sizeof(HashTable));
	if (hashTable == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Allocation */
	hashTable->size = numberOfBuckets;
	hashTable->storedRecs = 0;
	hashTable->buckets = malloc(sizeof(Bucket) * numberOfBuckets);
	if (hashTable->buckets == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	return hashTable;
}

void DestroyHT(HashTable *hashTable) {
	int i;
	CollisionList *nextRec, *delRec;
	for (i = 0; i < hashTable->size; i++) {
		nextRec = hashTable->buckets[i].headOfList;
		/* Delele collision list */
		while (nextRec != NULL) {
			delRec = nextRec;
			free(delRec->ptrToRecord->deprt);
			free(delRec->ptrToRecord->firstname);
			free(delRec->ptrToRecord->lastname);
			free(delRec->ptrToRecord);
			free(delRec);
			nextRec = nextRec->nextRecord;
		}
		hashTable->buckets[i].headOfList = NULL;
	}
	free(hashTable->buckets);
	return;
}

/* Return 0 when the record is stored successfully
 * Return -1 for system errors
 */
int InsertRecordHT(HashTable *hashTable, Record *record) {
	int bucketNumber = HashFunction(record->postcode, hashTable->size);
	CollisionList *newRec = malloc(sizeof(CollisionList));
	if (newRec == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	newRec->ptrToRecord = record;
	newRec->nextRecord = hashTable->buckets[bucketNumber].headOfList;
	hashTable->buckets[bucketNumber].headOfList = newRec;
	hashTable->storedRecs++;
	return 0;
}

/* Return 0 when the record is deleted successfully
 * Return -1 when the record is not found
 */
int DeleteRecordHT(HashTable *hashTable, Record *record) {
	int bucketNumber = HashFunction(record->postcode, hashTable->size);
	CollisionList *tempRec1 = hashTable->buckets[bucketNumber].headOfList;
	CollisionList *tempRec2 = NULL;
	/* If rec is in head of the list*/
	if (tempRec1 != NULL && tempRec1->ptrToRecord->studid == record->studid) {
		hashTable->buckets[bucketNumber].headOfList = tempRec1->nextRecord;
		hashTable->storedRecs--;
		free(tempRec1);
		return 0;
	}

	while (tempRec1 != NULL && tempRec1->ptrToRecord->studid != record->studid) {
		tempRec2 = tempRec1;
		tempRec1 = tempRec1->nextRecord;
	}
	if (tempRec1 == NULL) {
		return -1;
	}
	tempRec2->nextRecord = tempRec1->nextRecord;
	hashTable->storedRecs--;
	free(tempRec1);
	return 0;
}

float AveragePostCodeHT(HashTable *hashTable, int postcode) {
	int bucketNumber = HashFunction(postcode, hashTable->size);
	int numOfRecs = 0;
	float average, gpaSum = 0;
	CollisionList *tempRec = hashTable->buckets[bucketNumber].headOfList;
	while (tempRec != NULL) {
		if (tempRec->ptrToRecord->postcode == postcode) {
			numOfRecs++;
			gpaSum += tempRec->ptrToRecord->gpa;
		}
		tempRec = tempRec->nextRecord;
	}
	if (numOfRecs > 0) {
		average = gpaSum / numOfRecs;
		return average;
	} else {
		return 0;
	}
}

/* Print only the results
 * exit(1) for system errors*/
int KTOPAveragePostCodeHT(HashTable *hashTable, int postcode, int k) {
	int bucketNumber = HashFunction(postcode, hashTable->size);
	Record **array = malloc(sizeof(Record*) * k);
	if (array == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	int i, j, index;
	for (i = 0; i < k; i++) {
		array[i] = NULL;
	}
	CollisionList *tempRec = hashTable->buckets[bucketNumber].headOfList;
	while (tempRec != NULL) {
		if (tempRec->ptrToRecord->postcode == postcode) {
			i = 0;
			/* Find the position in the array
			 * The array must be ordered*/
			while (array[i] != NULL && i < k) {
				if (tempRec->ptrToRecord->gpa <= array[i]->gpa) {
					i++;
				} else {
					break;
				}
			}
			index = i;
			if (i < k) {
				/* Shift elements*/
				for (j = k - 1; j > i; j--) {
					array[j] = array[j - 1];
				}
				array[index] = tempRec->ptrToRecord;
			}
		}
		tempRec = tempRec->nextRecord;
	}
	for (i = 0; i < k; i++) {
		if (array[i] != NULL) {
			printf("%d %s %s %.2f %s %d\n", array[i]->studid, array[i]->lastname, array[i]->firstname, array[i]->gpa,
					array[i]->deprt, array[i]->numcourses);
			//fflush(stdout);
		}
	}
	free(array);
	return 0;
}

/* Print the students who live in postocode and study at deprt
 * Return 0 if students found
 * Return -1 otherwise
 */
int CoursesToTakeHT(HashTable *hashTable, int postcode, char *deprt) {
	int studentsFound = 0;
	int bucketNumber = HashFunction(postcode, hashTable->size);
	CollisionList *tempRec = hashTable->buckets[bucketNumber].headOfList;
	while (tempRec != NULL) {
		if (tempRec->ptrToRecord->postcode == postcode
				&& strncmp(tempRec->ptrToRecord->deprt, deprt, BUFFERSIZE) == 0) {
			studentsFound = 1;
			printf("%d %s %s %.2f %s %d\n", tempRec->ptrToRecord->studid, tempRec->ptrToRecord->lastname,
					tempRec->ptrToRecord->firstname, tempRec->ptrToRecord->gpa, tempRec->ptrToRecord->deprt,
					tempRec->ptrToRecord->numcourses);
		}
		tempRec = tempRec->nextRecord;
	}
	if (studentsFound == 1) {
		return 0;
	} else {
		return -1;
	}
}

/* Return the percentage of students who live in postcode
 * Return -1 if the hashtable is empty
 */
float PercentilePostCodeHT(HashTable *hashTable, int postcode) {
	if (hashTable->storedRecs == 0) {
		return -1;
	} else {
		int bucketNumber = HashFunction(postcode, hashTable->size);
		CollisionList *tempRec = hashTable->buckets[bucketNumber].headOfList;
		float percentage = 0, studCounter = 0;
		while (tempRec != NULL) {
			if (tempRec->ptrToRecord->postcode == postcode) {
				studCounter++;
			}
			tempRec = tempRec->nextRecord;
		}
		percentage = (float) studCounter / hashTable->storedRecs;
		return percentage;
	}
}

/* Print percentage of students who live per postcode
 * Return -1 if the hashtable is empty
 * exit(1) for system errors
 */

int PercentilesHT(HashTable *hashTable) {
	if (hashTable->storedRecs == 0) {
		return -1;
	} else {
		int iter, i, numOfStoredPostcodes, postFound;
		int sizeY;
		/* 2d dimension array to store in one column the postcodes
		 * and the number of studens in the other column
		 * postcodeArray[i][0] == postcode
		 * postcodeArray[i][1] == studcounter
		 */
		int **postcodeArray;
		CollisionList *tempRec;
		for (iter = 0; iter < hashTable->size; iter++) {
			/* When new bucket is iterated the postcodeArray recreates
			 * Allocate. At start is 2x2
			 */
			sizeY = 2;
			postcodeArray = (int **) calloc(2, sizeof(int*));
			for (i = 0; i < sizeY; i++) {
				postcodeArray[i] = (int *) calloc(sizeY, sizeof(int));
			}
			if (postcodeArray == NULL) {
				printf("calloc error %s\n", strerror(errno));
				exit(1);
			}
			numOfStoredPostcodes = 0;
			tempRec = hashTable->buckets[iter].headOfList;
			while (tempRec != NULL) {
				postFound = 0;
				/* Search the postcodeArray*/
				for (i = 0; i < sizeY; i++) {
					/* If the postcode is stored in the array increment by one
					 * the postcodeArray[i][1] element
					 */
					if (tempRec->ptrToRecord->postcode == postcodeArray[i][0]) {
						postcodeArray[i][1]++;
						postFound = 1;
						break;
					}
				}
				/* If the postcode is not saved in the array
				 * find an empty place and store it
				 * otherwise reallocate and double the size of the array
				 * and try to store it again
				 */
				if (postFound == 0) {
					/* If the array is full realloc*/
					if (numOfStoredPostcodes == sizeY) {
						int newSizeY = sizeY * 2;
						int **tempArray = (int **) realloc(postcodeArray, newSizeY * sizeof(int*));
						if (tempArray == NULL) {
							printf("realloc error %s\n", strerror(errno));
							exit(1);
						}
						postcodeArray = tempArray;
						for (i = sizeY; i < newSizeY; i++) {
							postcodeArray[i] = (int *) calloc(2, sizeof(int));
						}
						sizeY = newSizeY;
					}
					/* Find an empty place and store the postcode*/
					for (i = 0; i < sizeY; i++) {
						if (postcodeArray[i][0] == 0) {
							postcodeArray[i][0] = tempRec->ptrToRecord->postcode;
							postcodeArray[i][1]++;
							numOfStoredPostcodes++;
							break;
						}
					}
				}
				tempRec = tempRec->nextRecord;
			}
			float percentage;
			for (i = 0; i < sizeY; i++) {
				if (postcodeArray[i][0] != 0) {
					percentage = (float) postcodeArray[i][1] / hashTable->storedRecs;
					printf("%d %.2f\n", postcodeArray[i][0], percentage);
					fflush(stdout);
				}
			}
			for (i = 0; i < sizeY; i++) {
				free(postcodeArray[i]);
			}
			free(postcodeArray);
		}
	}
	return 0;
}
