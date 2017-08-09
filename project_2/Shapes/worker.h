/*
 * worker.h
 *
 *  Created on: Nov 6, 2016
 *      Author: vangelis
 */

#ifndef WORKER_H_
#define WORKER_H_

void WorkerCircle(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r, int offset, int pointsToRead);

void WorkerSemicircle(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r, char *direction, int offset,
		int pointsToRead);

void WorkerRing(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r1, char *r2, int offset,
		int pointsToRead);

void WorkerSquare(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r, int offset, int pointsToRead);

void WorkerEllipse(char *inputBinaryFile, char *outputFile, char *h, char *k, char *a, char *b, int offset,
		int pointsToRead);

#endif /* WORKER_H_ */
