/*
 * handler.c
 *
 *  Created on: Nov 6, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "handler.h"
#include "worker.h"

#define BUFFERSIZE 256

void Handler(char *command, char *inputBinaryFile, int workersCount, char *tempDir, int *offsets, int *pointsToRead) {
	int i;
	char *token;
	char **words = NULL;
	int numberOfSpaces = 0, tokenCounter = 0;
	/* Split the command into separate words*/
	token = strtok(command, " \t\n");
	while (token != NULL) {
		numberOfSpaces++;
		words = realloc(words, sizeof(char*) * numberOfSpaces);
		if (words == NULL) {
			printf("malloc error %s\n", strerror(errno));
			exit(1);
		}
		words[tokenCounter] = token;
		tokenCounter++;
		token = strtok(NULL, " \t\n");
	}
	int status;
	pid_t waitPID;
	pid_t pid = getpid();
	pid_t forkpid;
	char *outfile = (char *) malloc(sizeof(char*) * BUFFERSIZE);
	if (outfile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Concatenate the name of the tempdir with the SHPID.out file*/
	snprintf(outfile, BUFFERSIZE, "%s/%d.out", tempDir, pid);
	FILE *outFileDescr = fopen(outfile, "wb");
	int bytes_in, rc;
	char buf[BUFFERSIZE];
	struct pollfd fdarray[workersCount];
	char *fifoPath = (char *) malloc(sizeof(char*) * BUFFERSIZE);
	if (fifoPath == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Spawn workers an create FIFOs*/
	for (i = 0; i < workersCount; i++) {
		/* Create FIFO*/
		/* Concatenate the name of the tempdir with the SHPID_w(#).fifo file*/
		snprintf(fifoPath, BUFFERSIZE, "%s/%d_w%d", tempDir, pid, i);
		if (mkfifo(fifoPath, 0666) < 0) {
			printf("mkfifo error %s\n", strerror(errno));
			exit(1);
		}
		/* Store the file descriptors of the fifos using pollfd struct*/
		fdarray[i].fd = open(fifoPath, O_RDONLY | O_NONBLOCK, 0);
		fdarray[i].events = POLLIN;
		/* Create Workers*/
		forkpid = fork();
		if (forkpid < 0) {
			printf("fork error %s\n", strerror(errno));
			exit(1);
		}
		/* Worker*/
		else if (forkpid == 0) {
			/* Choose the correct shape*/
			if (strcmp(words[0], "circle") == 0) {
				WorkerCircle(inputBinaryFile, fifoPath, words[1], words[2], words[3], offsets[i], pointsToRead[i]);
			} else if (strcmp(words[0], "semicircle") == 0) {
				WorkerSemicircle(inputBinaryFile, fifoPath, words[1], words[2], words[3], words[4], offsets[i],
						pointsToRead[i]);
			} else if (strcmp(words[0], "ring") == 0) {
				WorkerRing(inputBinaryFile, fifoPath, words[1], words[2], words[3], words[4], offsets[i],
						pointsToRead[i]);
			} else if (strcmp(words[0], "square") == 0) {
				WorkerSquare(inputBinaryFile, fifoPath, words[1], words[2], words[3], offsets[i], pointsToRead[i]);
			} else if (strcmp(words[0], "ellipse") == 0) {
				WorkerEllipse(inputBinaryFile, fifoPath, words[1], words[2], words[3], words[4], offsets[i],
						pointsToRead[i]);
			}
		}
	}
	int breakLoop = 0;
	int time = workersCount * 500;
	/* This loop ends when every worker has finished its work with the calculations
	 * and the writing on its fifo.
	 * If a worker didn't found any points it will return "NO RESULTS\n"
	 */
	while (breakLoop < workersCount) {
		rc = poll(fdarray, workersCount, time);
		if (rc < 0) {
			printf("poll error %s\n", strerror(errno));
			exit(1);
		}
		for (i = 0; i < workersCount; i++) {
			if (fdarray[i].revents & POLLIN) {
				while (1) {
					/* Read from FIFO*/
					bytes_in = read(fdarray[i].fd, buf, BUFFERSIZE);
					/* If it is not empty then...*/
					if (bytes_in > 0) {
						buf[bytes_in] = '\0';
						/* If the worker send "NO RESULTS" increment the counter and break the loop*/
						if (strncmp(buf, "NO RESULTS\n", BUFFERSIZE) == 0) {
							breakLoop++;
							break;
						}
						/* Write the results, that the worker send, to the SHPID.out file*/
						else {
							fprintf(outFileDescr, "%s", buf);
							fflush(outFileDescr);
						}
					} else {
						breakLoop++;
						break;
					}
				}
			}

		}
	}
	/* WAIT FOR ALL THE WORKERS TO TERMINATE*/
	while ((waitPID = wait(&status)) > 0) {
	}
	/* Delete FIFOs*/
	for (i = 0; i < workersCount; i++) {
		snprintf(fifoPath, 256, "%s/%d_w%d", tempDir, pid, i);
		if (unlink(fifoPath) < 0) {
			printf("unlink error %s\n", strerror(errno));
			exit(1);
		}
	}
	fclose(outFileDescr);
	/* Free allocated space*/
	free(outfile);
	free(fifoPath);
	free(token);
	free(words);
	exit(0);
}
