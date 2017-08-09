/*
 * ellipse.h
 *
 *  Created on: Nov 4, 2016
 *      Author: vangelis
 */

#ifndef ELLIPSE_H_
#define ELLIPSE_H_

typedef struct Point {
	float x;
	float y;
} Point;

void Ellipse(float h, float k, float a, float b, char *inputBinaryFile, char *outputFile, int offset,
		int pointsToReadCount);

#endif /* ELLIPSE_H_ */
