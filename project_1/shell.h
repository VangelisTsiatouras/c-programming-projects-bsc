/*
 * shell.h
 *
 *  Created on: Oct 10, 2016
 *      Author: vangelis
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "hashtable.h"
#include "skiplist.h"

void shell(HashTable *hashTable, SkipListNode *skipListHead, char *operationsFile);

int ShellExecuteCommand(HashTable *hashTable, SkipListNode *skipListHead, char *lineBuffer);

void Insert(HashTable *hashTable, SkipListNode *skipListHead, int studid, char *lastname, char *firstname, float gpa,
		int numcourses, char *deprt, int postcode);

void Query(SkipListNode *skipListHead, int studid);

void Modify(SkipListNode *skipListHead, int studid, float gpa, int numcourses);

void Delete(HashTable *hashTable, SkipListNode *skipListHead, int studid);

void RAverage(SkipListNode *skipListHead, int studida, int studidb);

void Average(HashTable *hashTable, int postcode);

void TAverage(HashTable *hashTable, int postcode, int k);

void Bottom(SkipListNode *skipListHead, int k);

void CoursesToTake(HashTable *hashTable, int postcode, char *deprt);

void Find(SkipListNode *skipListHead, float gpa);

void Percentile(HashTable *hashTable, int postcode);

void PErcentiles(HashTable *hashTable);

#endif /* SHELL_H_ */
