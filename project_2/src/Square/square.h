/*
 * square.h
 *
 *  Created on: Nov 4, 2016
 *      Author: vangelis
 */

#ifndef SQUARE_H_
#define SQUARE_H_

typedef struct Point {
	float x;
	float y;
} Point;

void Square(float x, float y, float r, char *inputBinaryFile, char *outputFile, int offset, int pointsToReadCount);
float TriangleArea(float x1, float y1, float x2, float y2, float x3, float y3);

#endif /* SQUARE_H_ */
