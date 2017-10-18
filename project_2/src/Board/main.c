/*
 * main.c
 *
 *  Created on: Apr 13, 2016
 *      Author: vangelis
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <sys/stat.h>
#include "board_server.h"
#include "board_client.h"
#include "auxiliary_functions.h"
#include "definitions.h"

/* 								IMPORTANT!!!
 * THE DIRECTORY WHICH WILL CONTAIN ALL THE BOARDS MUST BE ALREADY CREATED!!
 * e.g. tmp/sdi1200185/board1 , the directory /sdi1200185 must be manually created
 * only the directory of the board is automatically created by this program!
*/

int main(int argc, char *argv[]) {
	if (argc > 2) {
		printf("too many arguments!\n");
		exit(1);
	} else if (argc == 1) {
		printf("too few arguments!\n");
		exit(1);
	}
	int pathIsAccepted = 0;
	int err = mkdir(argv[1], 0766);
	// Create a new directory for the new board
	if (err == 0) {
		pathIsAccepted = 1;
		printf("directory just created\n");
	}
	if (err < 0) {
		// If directory of the board exists
		if (errno == EEXIST) {
			printf("directory found\n");
			pathIsAccepted = 1;
		} else {
			perror("main: mkdir error\nDescription");
			exit(2);
		}
	}
	// Name of the board
	char *boardName = basename(argv[1]);
	printf("board name: %s\n", boardName);
	if (pathIsAccepted == 1) {
		pid_t pid = getpid();
		int serverIsDown = 0;
		char *pathToServerPID = malloc(BUFFERSIZE);
		if (pathToServerPID == NULL) {
			perror("main: malloc error\nDescription");
			exit(3);
		}
		FILE *serverPID;
		pathToServerPID = Concatenate(argv[1], boardName, "_Server_PID.txt");
		// If there isn't the file (boardName)_server_PID.txt then the server is down
		if (!(serverPID = fopen(pathToServerPID, "r"))) {
			serverIsDown = 1;
		}
		// Check if the pid in (boardName)_server_PID.txt is running, if it isn't then the server is down
		else {
			pid_t servpid;
			char buf[BUFFERSIZE];
			// Read the server_PID.txt to get the server pid
			while (fgets(buf, BUFFERSIZE, serverPID) != NULL) {
				servpid = atoi(buf);
			}
			// Sent a signal to the process with pid that equals with servpid
			// Process is running
			if (kill(servpid, 0) == 0) {
				printf("server is running\n");
			}
			// Process isn't running
			else if (errno == ESRCH) {
				serverIsDown = 1;
			}
			// Error
			else {
				perror("main: kill error\nDescription");
				exit(4);
			}
		}
		free(pathToServerPID);
		// Start the server
		if (serverIsDown == 1) {
			printf("server is not running!\n");
			printf("starting up server...\n");
			pid = fork();
		}
		// Server process
		if (pid == 0) {
			BoardServer(argv[1], boardName);
			exit(5);
		}
		// Client process
		else {
			BoardClientShell(argv[1], boardName);
			exit(5);
		}
	}
	exit(0);
}
