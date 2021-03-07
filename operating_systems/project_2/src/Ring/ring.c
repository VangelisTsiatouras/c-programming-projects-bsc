/*
 * ring.c
 *
 *  Created on: Nov 5, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "ring.h"

#define BUFFERSIZE 256

void Ring(float x, float y, float r1, float r2, char *inputBinaryFile, char *outputFile, int offset,
		int pointsToReadCount) {
	FILE *binFile, *outFile;
	Point pointToCheck;
	int i, found = 0;
	/* Open binary file*/
	binFile = fopen(inputBinaryFile, "rb");
	if (binFile == NULL) {
		printf("failure: file not found!\n");
		exit(1);
	}
	/* Open output file*/
	outFile = fopen(outputFile, "wb");
	/* Check if the offset is correct and does not point in wrong position
	 * ex. offset = 3 bytes
	 */
	if (offset % sizeof(Point) != 0) {
		printf("wrong offset\n");
		exit(1);
	}
	/* Go to, where the offset points in the bin file*/
	fseek(binFile, offset, SEEK_SET);
	for (i = 0; i < pointsToReadCount; i++) {
		fread(&pointToCheck, sizeof(Point), 1, binFile);
		/* The two circles equations*/
		if (((pow((x - pointToCheck.x), (float) 2) + pow((y - pointToCheck.y), (float) 2)) <= (pow(r1, (float) 2)))
				&& ((pow((x - pointToCheck.x), (float) 2) + pow((y - pointToCheck.y), (float) 2))
						>= (pow(r2, (float) 2)))) {
			fprintf(outFile, "	%.2f	%.2f\n", pointToCheck.x, pointToCheck.y);
			found++;
		}
	}
	/* If no point found to be in the ellipse the write NO RESULTS\n to output file*/
	if (found == 0) {
		fprintf(outFile, "NO RESULTS\n");
	}
	fclose(binFile);
	fclose(outFile);
}

