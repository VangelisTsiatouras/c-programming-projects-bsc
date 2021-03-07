/*
 * ring.h
 *
 *  Created on: Nov 5, 2016
 *      Author: vangelis
 */

#ifndef RING_H_
#define RING_H_

typedef struct Point {
	float x;
	float y;
} Point;

void Ring(float x, float y, float r1, float r2, char *inputBinaryFile, char *outputFile, int offset,
		int pointsToReadCount);

#endif /* RING_H_ */
