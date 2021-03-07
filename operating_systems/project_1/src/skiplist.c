/*
 * skiplist.c
 *
 *  Created on: Oct 9, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "skiplist.h"

SkipListNode* CreateSL() {
	/* Allocate the head node of the skip list*/
	SkipListNode* skipListHead = malloc(sizeof(SkipListNode));
	if (skipListHead == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	skipListHead->key = 0;
	skipListHead->ptrToRecord = NULL;
	skipListHead->forward = malloc(sizeof(SkipListNode *) * MAX_LEVEL);
	/* Allocate the term node of the skip list */
	SkipListNode *skipListTerm = malloc(sizeof(SkipListNode));
	if (skipListTerm == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	skipListTerm->key = 999999999;
	skipListTerm->ptrToRecord = NULL;
	skipListTerm->forward = malloc(sizeof(SkipListNode *) * MAX_LEVEL);
	int i;
	for (i = 0; i <= MAX_LEVEL - 1; i++) {
		skipListTerm->forward[i] = NULL;
	}
	/* Connect head and term nodes*/
	for (i = 0; i <= MAX_LEVEL - 1; i++) {
		skipListHead->forward[i] = skipListTerm;
	}
	return skipListHead;
}

void DestroySL(SkipListNode *skipListHead) {
	SkipListNode *tempNode1 = skipListHead;
	SkipListNode *tempNode2 = NULL;
	while (tempNode1 != NULL) {
		tempNode2 = tempNode1;
		tempNode1 = tempNode1->forward[0];
		free(tempNode2->forward);
		tempNode2->ptrToRecord = NULL;
		free(tempNode2);
	}
}

/* Return a pointer to new node when it is created successfully
 * exit(1) for system errors
 */
SkipListNode* CreateNode(int level, Record *record) {
	SkipListNode *newNode = malloc(sizeof(SkipListNode));
	if (newNode == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	newNode->key = record->studid;
	newNode->ptrToRecord = record;
	newNode->forward = malloc(sizeof(SkipListNode *) * MAX_LEVEL);
	int i;
	for (i = 0; i <= MAX_LEVEL - 1; i++) {
		newNode->forward[i] = NULL;
	}
	return newNode;
}

/* Return 0 if node is destroyed successfully */
int DestroyNode(SkipListNode *delNode) {
	free(delNode->forward);
	free(delNode);
	return 0;
}

/* Return 0 if node is inserted successfully
 * Return -1 if the node is already stored
 * exit(1) for system errors
 */
int InsertRecordSL(SkipListNode *skipListHead, Record *record) {
	SkipListNode **updateArray = malloc(sizeof(SkipListNode*) * (MAX_LEVEL - 1));
	if (updateArray == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	SkipListNode *tempNode = skipListHead;
	int i;
	for (i = MAX_LEVEL - 1; i >= 0; i--) {
		while (tempNode->forward[i]->key < record->studid) {
			tempNode = tempNode->forward[i];
		}
		updateArray[i] = tempNode;
	}
	tempNode = tempNode->forward[0];
	/* If the record is already stored*/
	if (tempNode->key == record->studid) {
		free(updateArray);
		return -1;
	} else {
		int level = rand() % (MAX_LEVEL) + 1;
		tempNode = CreateNode(level, record);
		/* Check if CreateNode succeeded */
		if (tempNode == NULL) {
			exit(1);
		}
		for (i = 0; i < level; i++) {
			tempNode->forward[i] = updateArray[i]->forward[i];
			updateArray[i]->forward[i] = tempNode;
		}
		free(updateArray);
		return 0;
	}
}

/* Return 0 if node is deleted successfully
 * Return -1 if node is not found
 * exit(1) for system errors
 */
int DeleteRecordSL(SkipListNode *skipListHead, int studID) {
	SkipListNode **updateArray = malloc(sizeof(SkipListNode*) * (MAX_LEVEL - 1));
	if (updateArray == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	SkipListNode *tempNode = skipListHead;
	int nodeDeleted = 1;
	int i;
	for (i = MAX_LEVEL - 1; i >= 0; i--) {
		while (tempNode->forward[i]->key < studID) {
			tempNode = tempNode->forward[i];
		}
		updateArray[i] = tempNode;
	}
	tempNode = tempNode->forward[0];
	if (tempNode->key == studID) {
		for (i = 0; i <= MAX_LEVEL - 1; i++) {
			if (updateArray[i]->forward[i] != tempNode) {
				break;
			}
			updateArray[i]->forward[i] = tempNode->forward[i];
		}
		/* Free update array */
		free(updateArray);
		/* Delete Node */
		nodeDeleted = 0;
		if ((DestroyNode(tempNode)) != 0) {
			exit(1);
		}
	}
	if (nodeDeleted == 0) {
		return 0;
	} else {
		return -1;
	}
}

/* Return a pointer to node when it is found
 * Return NULL when the studID does not exist
 */
Record* SearchReturnRecSL(SkipListNode *skipListHead, int studID) {
	SkipListNode *tempNode = skipListHead;
	int i;
	for (i = MAX_LEVEL - 1; i >= 0; i--) {
		while (tempNode->forward[i]->key < studID) {
			tempNode = tempNode->forward[i];
		}
	}
	tempNode = tempNode->forward[0];
	if (tempNode->key == studID) {
		return tempNode->ptrToRecord;
	} else {
		return NULL;
	}
}

/* Return 0 when studID is found
 * Return 1 the studID does not exist
 */
int SearchRecSL(SkipListNode *skipListHead, int studID) {
	SkipListNode *tempNode = skipListHead;
	int i;
	for (i = MAX_LEVEL - 1; i >= 0; i--) {
		while (tempNode->forward[i]->key < studID) {
			tempNode = tempNode->forward[i];
		}
	}
	tempNode = tempNode->forward[0];
	if (tempNode->key == studID) {
		return 0;
	} else {
		return 1;
	}
}

/* Return the average of students with studIDA-studIDB
 * If there is no one return 0
 */
float RAverageSL(SkipListNode *skipListHead, int studIDA, int studIDB) {
	SkipListNode *tempNode = skipListHead;
	int i, numOfRecs = 0;
	float raverage, gpaSum = 0;
	/* Find studIDA*/
	for (i = MAX_LEVEL - 1; i >= 0; i--) {
		while (tempNode->forward[i]->key <= studIDA) {
			tempNode = tempNode->forward[i];
		}
	}
	/* Iterate the nodes from studIA to stuIDB one by one*/
	tempNode = tempNode->forward[0];
	while (tempNode->key <= studIDB) {
		numOfRecs++;
		gpaSum += tempNode->ptrToRecord->gpa;
		tempNode = tempNode->forward[0];
	}
	if (numOfRecs > 0) {
		raverage = gpaSum / numOfRecs;
		return raverage;
	} else {
		return 0;
	}
}

/* Print the k students who have the minimum gpa
 * Return 0 if everything went good
 * Otherwise exit(1) (system errors)
 */
int PrintKBottomSL(SkipListNode *skipListHead, int k) {
	Record **bottomArray = malloc(sizeof(Record*) * k);
	if (bottomArray == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	int i, j, index;
	for (i = 0; i < k; i++) {
		bottomArray[i] = NULL;
	}
	SkipListNode *tempNode = skipListHead;
	while (tempNode != NULL) {
		if (tempNode != skipListHead && tempNode->key != 999999999) {
			i = 0;
			/* Find the position in the array
			 * The array must be ordered*/
			while (bottomArray[i] != NULL && i < k) {
				if (tempNode->ptrToRecord->gpa >= bottomArray[i]->gpa) {
					i++;
				} else {
					break;
				}
			}
			/* If i<k then the element shouldn't be placed in the ordered array */
			if (i < k) {
				index = i;
				/* Shift elements*/
				for (j = k - 1; j > i; j--) {
					bottomArray[j] = bottomArray[j - 1];
				}
				bottomArray[index] = tempNode->ptrToRecord;
			}
		}
		tempNode = tempNode->forward[0];
	}
	for (i = 0; i < k; i++) {
		if (bottomArray[i] != NULL) {
			printf("%d %s %s %.2f %s %d\n", bottomArray[i]->studid, bottomArray[i]->lastname, bottomArray[i]->firstname,
					bottomArray[i]->gpa, bottomArray[i]->deprt, bottomArray[i]->numcourses);
		}
		//fflush(stdout);
	}
	free(bottomArray);
	return 0;
}

/* Print the students who have the max number of courses and gpa >= given gpa
 * Return 0 if everything went good
 */
int FindGPACoursesSL(SkipListNode *skipListHead, float gpa) {
	SkipListNode *tempNode = skipListHead;
	int maxCourses = 0;
	/* Find the maximum number of courses-to-take*/
	while (tempNode != NULL) {
		if (tempNode != skipListHead && tempNode->key != 999999999) {
			if (tempNode->ptrToRecord->numcourses > maxCourses) {
				maxCourses = tempNode->ptrToRecord->numcourses;
			}
		}
		tempNode = tempNode->forward[0];
	}
	/* Find the students with numcourses == maxCourses
	 * AND gpa greater than the given from the input*/
	tempNode = skipListHead;
	while (tempNode != NULL) {
		if (tempNode != skipListHead && tempNode->key != 999999999) {
			if (tempNode->ptrToRecord->numcourses == maxCourses && tempNode->ptrToRecord->gpa >= gpa) {
				printf("%d %s %s %.2f %d %s %d\n", tempNode->ptrToRecord->studid, tempNode->ptrToRecord->lastname,
						tempNode->ptrToRecord->firstname, tempNode->ptrToRecord->gpa, tempNode->ptrToRecord->numcourses,
						tempNode->ptrToRecord->deprt, tempNode->ptrToRecord->postcode);
			}
		}
		tempNode = tempNode->forward[0];
	}
	return 0;
}
