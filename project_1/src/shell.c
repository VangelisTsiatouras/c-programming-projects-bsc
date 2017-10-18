/*
 * shell.c
 *
 *  Created on: Mar 15, 2016
 *      Author: vangelis
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shell.h"

#define BUFFERSIZE 256

void shell(int numberOfBuckets, char* operationFile) {
	char buf[BUFFERSIZE];
	HashTable* hashTable = CreateHashtable(numberOfBuckets);
	if (hashTable == NULL) {
		printf("failure: hashtable could not be created!\n");
		return;
	}
	//Check if there is an operation file
	if (operationFile != NULL) {
		FILE* opFile;
		opFile = fopen(operationFile, "r");
		if (opFile == NULL) {
			printf("failure: file not found!\n");
			return;
		}
		//Read the operation file line by line
		while (fgets(buf, BUFFERSIZE, opFile) != NULL) {
			ShellExecuteCommand(buf, hashTable);
		}
	}
	//Now read from stdin
	while (fgets(buf, BUFFERSIZE, stdin) != NULL) {
		ShellExecuteCommand(buf, hashTable);
	}
	DestroyHashtable(hashTable);
}

void ShellExecuteCommand(char* lineBuffer, HashTable* hashTable) {
	char* token;
	char** words = NULL;
	int numberOfSpaces = 0, tokencounter = 0;
	int i;
	//Split the entered line to words
	token = strtok(lineBuffer, " \t");
	while (token != NULL) {
		numberOfSpaces++;
		words = realloc(words, sizeof(char*) * numberOfSpaces);
		if (words == NULL) {
			printf("MALLOC ERROR!\n");
			return;
		}
		words[tokencounter] = token;
		tokencounter++;
		token = strtok(NULL, " \t");
	}
	//Execute given command
	int charToNum1, charToNum2;
	float charToFloat;
	if (strcmp(words[0], "createnodes") == 0) {
		if (tokencounter == 1) {
			printf("too few arguments!\n");
			return;
		}
		for (i = 1; i < tokencounter; i++) {
			charToNum1 = atoi(words[i]);
			if (charToNum1 != 0) {
				AddNode(hashTable, charToNum1);
			}
		}
	} else if (strcmp(words[0], "delnodes") == 0) {
		if (tokencounter == 1) {
			printf("too few arguments!\n");
			return;
		}
		for (i = 1; i < tokencounter; i++) {
			charToNum1 = atoi(words[i]);
			if (charToNum1 != 0) {
				DeleteNode(hashTable, charToNum1);
			}
		}
	} else if (strcmp(words[0], "addtran") == 0) {
		if (tokencounter <= 3) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 4) {
			printf("too many arguments!\n");
			return;
		}
		//N1
		charToNum1 = atoi(words[1]);
		//N2
		charToNum2 = atoi(words[2]);
		//amount
		charToFloat = atof(words[3]);
		if (charToNum1 != 0 && charToNum2 != 0 && charToFloat != 0) {
			AddTran(hashTable, charToNum1, charToNum2, charToFloat);
		}
	} else if (strcmp(words[0], "deltran") == 0) {
		if (tokencounter <= 2) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 3) {
			printf("too many arguments!\n");
			return;
		}
		//N1
		charToNum1 = atoi(words[1]);
		//N2
		charToNum2 = atoi(words[2]);
		if (charToNum1 != 0 && charToNum2 != 0) {
			DeleteTran(hashTable, charToNum1, charToNum2);
		}
	} else if (strcmp(words[0], "lookup") == 0) {
		if (tokencounter <= 2) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 3) {
			printf("too many arguments!\n");
			return;
		}
		//N1
		charToNum1 = atoi(words[2]);
		//words[1] == [in | out | sum]
		if (charToNum1 != 0) {
			Lookup(hashTable, charToNum1, words[1]);
		}
	} else if (strcmp(words[0], "triangle") == 0) {
		if (tokencounter <= 2) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 3) {
			printf("too many arguments!\n");
			return;
		}
		//N
		charToNum1 = atoi(words[1]);
		//k
		charToFloat = atof(words[2]);
		if (charToNum1 != 0 && charToFloat != 0) {
			Triangle(hashTable, charToNum1, charToFloat);
		}
	} else if (strcmp(words[0], "conn") == 0) {
		if (tokencounter <= 2) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 3) {
			printf("too many arguments!\n");
			return;
		}
		//N
		charToNum1 = atoi(words[1]);
		//k
		charToNum2 = atoi(words[2]);
		if (charToNum1 != 0 && charToNum2 != 0) {
			Connection(hashTable, charToNum1, charToNum2);
		}
	} else if (strcmp(words[0], "allcycles") == 0) {
		if (tokencounter <= 1) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 2) {
			printf("too many arguments!\n");
			return;
		}
		//N
		charToNum1 = atoi(words[1]);
		if (charToNum1 != 0) {
			AllCycles(hashTable, charToNum1);
		}
	} else if (strcmp(words[0], "traceflow") == 0) {
		if (tokencounter <= 2) {
			printf("too few arguments!\n");
			return;
		}
		if (tokencounter > 3) {
			printf("too many arguments!\n");
			return;
		}
		//N
		charToNum1 = atoi(words[1]);
		//I
		charToNum2 = atoi(words[2]);
		if (charToNum1 != 0 && charToNum2 != 0) {
			Traceflow(hashTable, charToNum1, charToNum2);
		}
	} else if (strcmp(words[0], "bye\n") == 0) {
		if (tokencounter > 1) {
			printf("too many arguments!\n");
			return;
		}
		Bye(hashTable);
	} else if (strcmp(words[0], "print\n") == 0) {
		if (tokencounter > 1) {
			printf("too many arguments!\n");
			return;
		}
		PrintHash(hashTable);
	} else if (strcmp(words[0], "\n") == 0) {
		printf("\n");
	} else {
		printf("unknown command\n");
		return;
	}
	free(token);
	free(words);
}
