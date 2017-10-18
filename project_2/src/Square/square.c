/*
 * square.c
 *
 *  Created on: Nov 4, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "square.h"

#define BUFFERSIZE 256

void Square(float x, float y, float r, char *inputBinaryFile, char *outputFile, int offset, int pointsToReadCount) {
	FILE *binFile, *outFile;
	Point pointToCheck, A, B, C, D;
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
	/* Locate the edges of the square*/
	A.x = x - r;
	A.y = y;
	B.x = x;
	B.y = y + r;
	C.x = x + r;
	C.y = y;
	D.x = x;
	D.y = y - r;
	float areaAPB, areaBPC, areaCPD, areaDPA;
	float squareEdge = sqrt(pow((A.x - B.x), (float) 2) + pow((A.y - B.y), (float) 2));
	for (i = 0; i < pointsToReadCount; i++) {
		fread(&pointToCheck, sizeof(Point), 1, binFile);
		/* HERON'S FORMULA*/
		/* Triangle APB*/
		areaAPB = TriangleArea(A.x, A.y, pointToCheck.x, pointToCheck.y, B.x, B.y);
		/* Triangle BPC*/
		areaBPC = TriangleArea(B.x, B.y, pointToCheck.x, pointToCheck.y, C.x, C.y);
		/* Triangle CPD*/
		areaCPD = TriangleArea(C.x, C.y, pointToCheck.x, pointToCheck.y, D.x, D.y);
		/* Triangle DPA*/
		areaDPA = TriangleArea(D.x, D.y, pointToCheck.x, pointToCheck.y, A.x, A.y);
		if ((areaAPB + areaBPC + areaCPD + areaDPA) <= pow(squareEdge, (float) 2) + 0.01) {
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

float TriangleArea(float x1, float y1, float x2, float y2, float x3, float y3) {
	/* HERON's FORMULA
	 * Area = sqr(s(s-a)(s-b)(s-c)
	 * a,b,c := distances of the edges
	 * s := semiperimeter of the triangle/ s = (a+b+c)/2
	 */
	float distance1 = sqrt(pow((x1 - x2), (float) 2) + pow((y1 - y2), (float) 2));
	float distance2 = sqrt(pow((x2 - x3), (float) 2) + pow((y2 - y3), (float) 2));
	float distance3 = sqrt(pow((x1 - x3), (float) 2) + pow((y1 - y3), (float) 2));
	float s = (distance1 + distance2 + distance3) / (float) 2;
	float area = sqrt(s * (s - distance1) * (s - distance2) * (s - distance3));
	return area;
}

