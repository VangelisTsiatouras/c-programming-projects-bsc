/*
 * worker.c
 *
 *  Created on: Nov 6, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "worker.h"

#define BUFFERSIZE 256

void WorkerCircle(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r, int offset, int pointsToRead) {
	/* Convert offset and points to read into characters*/
	char chOffset[BUFFERSIZE];
	char chPointsToRead[BUFFERSIZE];
	snprintf(chOffset, BUFFERSIZE, "%d", offset);
	snprintf(chPointsToRead, BUFFERSIZE, "%d", pointsToRead);
	/* Call execlp*/
	if (execlp("../Circle/circle", "circle", "-i", inputBinaryFile, "-o", outputFile, "-a",
			x, y, r, "-f", chOffset, "-n", chPointsToRead, NULL) < 0) {
		printf("execlp error %s\n", strerror(errno));
		exit(1);
	}
	exit(0);
}

void WorkerSemicircle(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r, char *direction, int offset,
		int pointsToRead) {
	/* Convert offset and points to read into characters*/
	char chOffset[BUFFERSIZE];
	char chPointsToRead[BUFFERSIZE];
	snprintf(chOffset, BUFFERSIZE, "%d", offset);
	snprintf(chPointsToRead, BUFFERSIZE, "%d", pointsToRead);
	/* Call execlp*/
	if (execlp("../Semicircle/semicircle", "semicircle", "-i", inputBinaryFile, "-o", outputFile,
			"-a", x, y, r, direction, "-f", chOffset, "-n", chPointsToRead, NULL) < 0) {
		printf("execlp error %s\n", strerror(errno));
		exit(1);
	}
	exit(0);
}

void WorkerRing(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r1, char *r2, int offset,
		int pointsToRead) {
	char chOffset[BUFFERSIZE];
	char chPointsToRead[BUFFERSIZE];
	snprintf(chOffset, BUFFERSIZE, "%d", offset);
	snprintf(chPointsToRead, BUFFERSIZE, "%d", pointsToRead);
	/* Call execlp*/
	if (execlp("../Ring/ring", "ring", "-i", inputBinaryFile, "-o", outputFile, "-a", x,
			y, r1, r2, "-f", chOffset, "-n", chPointsToRead, NULL) < 0) {
		printf("execlp error %s\n", strerror(errno));
		exit(1);
	}
	exit(0);
}

void WorkerSquare(char *inputBinaryFile, char *outputFile, char *x, char *y, char *r, int offset, int pointsToRead) {
	/* Convert offset and points to read into characters*/
	char chOffset[BUFFERSIZE];
	char chPointsToRead[BUFFERSIZE];
	snprintf(chOffset, BUFFERSIZE, "%d", offset);
	snprintf(chPointsToRead, BUFFERSIZE, "%d", pointsToRead);
	/* Call execlp*/
	if (execlp("../Square/square", "square", "-i", inputBinaryFile, "-o", outputFile, "-a",
			x, y, r, "-f", chOffset, "-n", chPointsToRead, NULL) < 0) {
		printf("execlp error %s\n", strerror(errno));
		exit(1);
	}
	exit(0);
}

void WorkerEllipse(char *inputBinaryFile, char *outputFile, char *h, char *k, char *a, char *b, int offset,
		int pointsToRead) {
	/* Convert offset and points to read into characters*/
	char chOffset[BUFFERSIZE];
	char chPointsToRead[BUFFERSIZE];
	snprintf(chOffset, BUFFERSIZE, "%d", offset);
	snprintf(chPointsToRead, BUFFERSIZE, "%d", pointsToRead);
	/* Call execlp*/
	if (execlp("../Ellipse/ellipse", "ellipse", "-i", inputBinaryFile, "-o", outputFile, "-a",
			h, k, a, b, "-f", chOffset, "-n", chPointsToRead, NULL) < 0) {
		printf("execlp error %s\n", strerror(errno));
		exit(1);
	}
	exit(0);
}
