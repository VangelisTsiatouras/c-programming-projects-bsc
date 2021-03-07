/*
 * circle.c
 *
 *  Created on: Nov 4, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "circle.h"

#define BUFFERSIZE 256

void Circle(float x, float y, float r, char *inputBinaryFile, char *outputFile, int offset, int pointsToReadCount) {
	FILE *binFile, *outFile;
	Point pointToCheck;
	int i, found = 0;
	/* Open binary file*/
	binFile = fopen(inputBinaryFile, "rb");
	if (binFile == NULL) {
		printf("file not found!\n");
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
		/* Circle equation*/
		if ((pow((x - pointToCheck.x), (float) 2) + pow((y - pointToCheck.y), (float) 2)) <= (pow(r, (float) 2))) {
			/* Write the point to output file*/
			fprintf(outFile, "	%.2f	%.2f\n", pointToCheck.x, pointToCheck.y);
			found++;
		}
	}
	/* If no point found to be in the circle the write NO RESULTS\n to output file*/
	if(found == 0){
		fprintf(outFile, "NO RESULTS\n");
	}
	fclose(binFile);
	fclose(outFile);
}
