/*
 * main.c
 *
 *  Created on: Mar 3, 2016
 *      Author: vangelis
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shell.h"

int main(int argc, char **argv) {
	int i, negativeNumberOfBuckets = 0, numberOfBucketsNotEntered = 0, operationFileNotEntered = 0, noMinusBFlag = 1;
	int numberOfBuckets = 0;
	char* operationFile = NULL;
	//Split the arguments
	if (argc > 5) {
		printf("too many arguments!\n");
	} else if (argc == 1) {
		printf("too few arguments!\n");
	} else {
		for (i = 0; i < argc; ++i) {
			if (strcmp(argv[i], "-b") == 0) {
				noMinusBFlag = 0;
				if (argv[i + 1] != NULL) {
					if (strcmp(argv[i + 1], "-o") != 0) {
						numberOfBuckets = atoi(argv[i + 1]);
						if (numberOfBuckets <= 0) {
							negativeNumberOfBuckets = 1;
						}
					} else {
						numberOfBucketsNotEntered = 1;
					}
				} else {
					numberOfBucketsNotEntered = 1;
				}
			}
			if (strcmp(argv[i], "-o") == 0) {
				if (argv[i + 1] != NULL) {
					if (strcmp(argv[i + 1], "-b") != 0) {
						operationFile = malloc(sizeof(char) * (strlen(argv[i + 1]) - 1)); /*TODO ERROR!!!*/
						if (operationFile == NULL) {
							printf("MALLOC ERROR!\n");
							return 1;
						}
						strcpy(operationFile, argv[i + 1]);
					} else {
						operationFileNotEntered = 1;
					}
				} else {
					operationFileNotEntered = 1;
				}
			}
		}
		if (noMinusBFlag == 1) {
			printf("flag -b is missing.\ntype -b [SIZE_OF_HASH] \nprogram terminated\n");
			return 1;
		}
		if (negativeNumberOfBuckets == 1) {
			printf("the size of the hashtable must be greater than 0\nprogram terminated\n");
			return 1;
		}
		if (numberOfBucketsNotEntered == 1) {
			printf(
					"too few arguments! Give a value to initialize the hashtable\ntype -b [SIZE_OF_HASH]\nprogram terminated\n");
			return 1;
		}
		if (operationFileNotEntered == 1) {
			printf(
					"too few arguments! Give a path of an operation file\ntype -o [PATH_OF_OPERATIONSFILE]\nprogram terminated\n");
			return 1;
		}
		shell(numberOfBuckets, operationFile);
	}
	free(operationFile);
	printf("program terminated\n");
	return 0;
}
