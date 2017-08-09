/*
 * circle.h
 *
 *  Created on: Nov 4, 2016
 *      Author: vangelis
 */

#ifndef CIRCLE_H_
#define CIRCLE_H_

typedef struct Point {
	float x;
	float y;
} Point;

void Circle(float x, float y, float r, char *inputBinaryFile, char *outputFile, int offset, int pointsToReadCount);

#endif /* CIRCLE_H_ */
