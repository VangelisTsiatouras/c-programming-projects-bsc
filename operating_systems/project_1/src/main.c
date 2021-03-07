/*
 * main.c
 *
 *  Created on: Oct 9, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "hashtable.h"
#include "skiplist.h"
#include "shell.h"

#define BUFFERSIZE 256

int main(int argc, char **argv) {
	int i, minusL = 0, minusB = 0, NOoperations_file = 0, NOhashentries = 0, NegativeHashentries = 0;
	int hashEntries;
	char* operationsFile = malloc(sizeof(char) * BUFFERSIZE);
	if (operationsFile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Split args */
	if (argc > 5) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 5) {
		printf("too few arguments!\n");
		return 1;
	} else {
		for (i = 0; i < argc; i++) {
			if (strncmp(argv[i], "-l", 2) == 0) {
				minusL = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-b", 2) != 0) {
					strncpy(operationsFile, argv[i + 1], BUFFERSIZE);
				} else {
					NOoperations_file = 1;
				}
			} else if (strncmp(argv[i], "-b", 2) == 0) {
				minusB = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-l", 2) != 0) {
					hashEntries = atoi(argv[i + 1]);
					if (hashEntries <= 0) {
						NegativeHashentries = 1;
					}
				} else {
					NOhashentries = 1;
				}
			}
		}
		if (minusL == 0) {
			printf("flag -l is missing.\ntype -l [OPERATIONSFILE] \nprogram terminated\n");
			return 1;
		}
		if (minusB == 0) {
			printf("flag -b is missing.\ntype -b [HASHENTRIES] \nprogram terminated\n");
			return 1;
		}
		if (NOoperations_file == 1) {
			printf("operations file not entered\ntype -l [OPERATIONSFILE] \nprogram terminated\n");
			return 1;
		}
		if (NOhashentries == 1) {
			printf("hashentries not entered\ntype -b [HASHENTRIES] \nprogram terminated\n");
			return 1;
		}
		if (NegativeHashentries == 1) {
			printf("hashentries must be greater than 0\n program terminated\n");
			return 1;
		}
	}
	/* Create structures */
	HashTable *hashTable = CreateHT(hashEntries);
	SkipListNode *skipListHead = CreateSL(&skipListHead);
	/* Execute commands from file/stdin*/
	shell(hashTable, skipListHead, operationsFile);
	/* Destroy structures */
	free(operationsFile);
	printf("program terminated\n");
	return 0;
}
