/*
 * main.c
 *
 *  Created on: Nov 5, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "master.h"

#define BUFFERSIZE 256

int main(int argc, char **argv) {
	int i, minusI = 0, minusW = 0, minusD = 0, NOinputBinaryFile = 0, NOworkersCount, NOtempDir = 0;
	int workersCount;
	char *inputBinaryFile = malloc(sizeof(char) * BUFFERSIZE);
	if (inputBinaryFile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	char *tempDir = malloc(sizeof(char) * BUFFERSIZE);
	if (tempDir == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Split args */
	if (argc > 7) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 7) {
		printf("too few arguments!\n");
		return 1;
	} else {
		/* If the arguments are not in specific order this loop filters them*/
		for (i = 0; i < argc; i++) {
			if (strncmp(argv[i], "-i", 2) == 0) {
				minusI = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-w", 2) != 0 && strncmp(argv[i + 1], "-d", 2) != 0) {
					strncpy(inputBinaryFile, argv[i + 1], BUFFERSIZE);
				} else {
					NOinputBinaryFile = 1;
				}
			} else if (strncmp(argv[i], "-w", 2) == 0) {
				minusW = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-i", 2) != 0 && strncmp(argv[i + 1], "-d", 2) != 0) {
					workersCount = atoi(argv[i + 1]);
				} else {
					NOworkersCount = 1;
				}
			} else if (strncmp(argv[i], "-d", 2) == 0) {
				minusD = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-i", 2) != 0 && strncmp(argv[i + 1], "-w", 2) != 0) {
					strncpy(tempDir, argv[i + 1], BUFFERSIZE);
				} else {
					NOtempDir = 1;
				}
			}
		}
		if (minusI == 0) {
			printf("flag -i is missing.\ntype -i [INPUTBINARYFILE] \nprogram terminated\n");
			return 1;
		}
		if (minusW == 0) {
			printf("flag -w is missing.\ntype -w [WORKERSCOUNT] \nprogram terminated\n");
			return 1;
		}
		if (minusD == 0) {
			printf("flag -d is missing.\ntype -d [TEMPDIR] \nprogram terminated\n");
			return 1;
		}
		if (NOinputBinaryFile == 1) {
			printf("binary file not entered\ntype -i [INPUTBINARYFILE] \nprogram terminated\n");
			return 1;
		}
		if (NOworkersCount == 1) {
			printf("workers count not entered\ntype -w [WORKERSCOUNT] \nprogram terminated\n");
			return 1;
		}
		if (NOtempDir == 1) {
			printf("temp dir not entered\ntype -d [TEMPDIR] \nprogram terminated\n");
			return 1;
		}
		Master(inputBinaryFile, workersCount, tempDir);
		free(inputBinaryFile);
		free(tempDir);
	}
}
