/*
 * main.c
 *
 *  Created on: Nov 4, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ellipse.h"

#define BUFFERSIZE 256

int main(int argc, char **argv) {
	int i, minusI = 0, minusO = 0, minusA = 0, minusF = 0, minusN = 0, NOinputBinaryFile = 0, NOoutputFile = 0,
			NOutilityArgs = 0, NOoffset = 0, NOpointsToRead = 0;
	float h, k, a, b;
	int offset, pointsToReadCount;
	char *inputBinaryFile = malloc(sizeof(char) * BUFFERSIZE);
	if (inputBinaryFile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	char *outputFile = malloc(sizeof(char) * BUFFERSIZE);
	if (outputFile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Split args */
	if (argc > 14) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 10) {
		printf("too few arguments!\n");
		return 1;
	} else {
		/* If the arguments are not in specific order this loop filters them*/
		for (i = 0; i < argc; i++) {
			if (strncmp(argv[i], "-i", 2) == 0) {
				minusI = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-o", 2) != 0 && strncmp(argv[i + 1], "-a", 2) != 0
						&& strncmp(argv[i + 1], "-f", 2) != 0 && strncmp(argv[i + 1], "-n", 2) != 0) {
					strncpy(inputBinaryFile, argv[i + 1], BUFFERSIZE);
				} else {
					NOinputBinaryFile = 1;
				}
			} else if (strncmp(argv[i], "-o", 2) == 0) {
				minusO = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-i", 2) != 0 && strncmp(argv[i + 1], "-a", 2) != 0
						&& strncmp(argv[i + 1], "-f", 2) != 0 && strncmp(argv[i + 1], "-n", 2) != 0) {
					strncpy(outputFile, argv[i + 1], BUFFERSIZE);
				} else {
					NOoutputFile = 1;
				}
			} else if (strncmp(argv[i], "-a", 2) == 0) {
				minusA = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-i", 2) != 0 && strncmp(argv[i + 1], "-o", 2) != 0
						&& strncmp(argv[i + 1], "-f", 2) != 0 && strncmp(argv[i + 1], "-n", 2) != 0
						&& strncmp(argv[i + 2], "-i", 2) != 0 && strncmp(argv[i + 2], "-o", 2) != 0
						&& strncmp(argv[i + 2], "-f", 2) != 0 && strncmp(argv[i + 2], "-n", 2) != 0
						&& strncmp(argv[i + 3], "-i", 2) != 0 && strncmp(argv[i + 3], "-o", 2) != 0
						&& strncmp(argv[i + 3], "-f", 2) != 0 && strncmp(argv[i + 3], "-n", 2) != 0
						&& strncmp(argv[i + 4], "-i", 2) != 0 && strncmp(argv[i + 4], "-o", 2) != 0
						&& strncmp(argv[i + 4], "-f", 2) != 0 && strncmp(argv[i + 4], "-n", 2) != 0) {
					h = atof(argv[i + 1]);
					k = atof(argv[i + 2]);
					a = atof(argv[i + 3]);
					b = atof(argv[i + 4]);
				} else {
					NOutilityArgs = 1;
				}
			} else if (strncmp(argv[i], "-f", 2) == 0) {
				minusF = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-i", 2) != 0 && strncmp(argv[i + 1], "-o", 2) != 0
						&& strncmp(argv[i + 1], "-a", 2) != 0 && strncmp(argv[i + 1], "-n", 2) != 0) {
					offset = atoi(argv[i + 1]);
				} else {
					NOoffset = 1;
				}
			} else if (strncmp(argv[i], "-n", 2) == 0) {
				minusN = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-i", 2) != 0 && strncmp(argv[i + 1], "-o", 2) != 0
						&& strncmp(argv[i + 1], "-a", 2) != 0 && strncmp(argv[i + 1], "-f", 2) != 0) {
					pointsToReadCount = atoi(argv[i + 1]);
				} else {
					NOpointsToRead = 1;
				}
			}
		}
		if (minusI == 0) {
			printf("flag -i is missing.\ntype -i [INPUTBINARYFILE] \nprogram terminated\n");
			return 1;
		}
		if (minusO == 0) {
			printf("flag -o is missing.\ntype -o [OUTPUTFILE] \nprogram terminated\n");
			return 1;
		}
		if (minusA == 0) {
			printf("flag -a is missing.\ntype -a [UTILITYARGS] \nprogram terminated\n");
			return 1;
		}
		if (NOinputBinaryFile == 1) {
			printf("binary file not entered\ntype -i [INPUTBINARYFILE] \nprogram terminated\n");
			return 1;
		}
		if (NOoutputFile == 1) {
			printf("output file not entered\ntype -o [OUTPUTFILE] \nprogram terminated\n");
			return 1;
		}
		if (NOutilityArgs == 1) {
			printf("utility args not entered\ntype -a [UTILITYARGS] \nprogram terminated\n");
			return 1;
		}
		/* Optionals*/
		/* If the offset is not set by the input
		 * initialise it with 0 (the start of the binary file)
		 */
		if (minusF == 0) {
			offset = 0;
		}
		/* If the pointsToRead is not set by the input
		 * initialise it with total the number of points
		 * that contain the binary file
		 */
		if (minusN == 0) {
			FILE *filepointer;
			long lSize;
			filepointer = fopen(inputBinaryFile, "rb");
			if (filepointer == NULL) {
				printf("Cannot open binary file\n");
				return 1;
			}
			fseek(filepointer, 0, SEEK_END);
			lSize = ftell(filepointer);
			rewind(filepointer);
			pointsToReadCount = (int) lSize / sizeof(Point);
			fclose(filepointer);
		}
		if (NOoffset == 1) {
			printf("offset not entered\ntype -f [OFFSET] \nprogram terminated\n");
			return 1;
		}
		if (NOpointsToRead == 1) {
			printf("points to read not entered\ntype -n [POINTSTOREAD] \nprogram terminated\n");
			return 1;
		}
	}
	Ellipse(h, k, a, b, inputBinaryFile, outputFile, offset, pointsToReadCount);
	free(inputBinaryFile);
	free(outputFile);
}

