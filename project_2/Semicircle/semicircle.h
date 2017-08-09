/*
 * semicircle.h
 *
 *  Created on: Nov 5, 2016
 *      Author: vangelis
 */

#ifndef SEMICIRCLE_H_
#define SEMICIRCLE_H_

typedef struct Point {
	float x;
	float y;
} Point;

void Semicircle(float x, float y, float r, char direction, char *inputBinaryFile, char *outputFile, int offset,
		int pointsToReadCount);

#endif /* SEMICIRCLE_H_ */
