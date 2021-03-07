/*
 * skiplist.h
 *
 *  Created on: Oct 9, 2016
 *      Author: vangelis
 */

#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include "record.h"

#define MAX_LEVEL 5

typedef struct SkipListNode {
	int key;
	Record *ptrToRecord;
	struct SkipListNode **forward;
}SkipListNode;

/* FUNCTIONS*/

SkipListNode* CreateSL();

void DestroySL(SkipListNode *skipListHead);

SkipListNode* CreateNode(int level, Record *record);

int DestroyNode(SkipListNode *delNode);

int InsertRecordSL(SkipListNode *skipListHead, Record *record);

int DeleteRecordSL(SkipListNode *skipListHead, int studID);

Record* SearchReturnRecSL(SkipListNode *skipListHead, int studID);

int SearchRecSL(SkipListNode *skipListHead, int studID);

float RAverageSL(SkipListNode *skipListHead, int studIDA, int studIDB);

int PrintKBottomSL(SkipListNode *skipListHead, int k);

int FindGPACoursesSL(SkipListNode *skipListHead, float gpa);

#endif /* SKIPLIST_H_ */
