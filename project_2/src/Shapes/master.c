/*
 * master.c
 *
 *  Created on: Nov 5, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "master.h"
#include "handler.h"
#include "point.h"

#define BUFFERSIZE 256
#define WORDSIZE 32

void Master(char *inputBinaryFile, int workersCount, char *tempDir) {
	/*Create temp DIR */
	int err = mkdir(tempDir, 0766);
	if (err == 0) {
		printf("directory with name: '%s' just created\n", tempDir);
	}
	if (err < 0) {
		/* If directory exists */
		if (errno == EEXIST) {
			printf("directory with name: '%s' already exists\nusing this directory\n", tempDir);
		} else {
			printf("mkdir error %s\n", strerror(errno));
			exit(1);
		}
	}
	/* Scan binary file to calculate number of points and offsets*/
	FILE *filepointer = fopen(inputBinaryFile, "rb");
	if (filepointer == NULL) {
		printf("fopen error %s\n", strerror(errno));
		exit(1);
	}
	long lSize;
	int numOfPoints, i, divPoints, modPoints, restPoints, tempOffset = 0;
	int *offsets = (int *) malloc(sizeof(int) * workersCount);
	if (offsets == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	int *pointsToRead = (int *) malloc(sizeof(int) * workersCount);
	if (pointsToRead == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Find how many points has the file*/
	fseek(filepointer, 0, SEEK_END);
	lSize = ftell(filepointer);
	rewind(filepointer);
	numOfPoints = (int) lSize / sizeof(Point);
	/* If the modulo is zero then the workers can split equally the file
	 * Otherwise the last worker must manipulate the remaining points,
	 * from the division with upper integer part
	 */
	modPoints = numOfPoints % (workersCount);
	if (modPoints == 0) {
		divPoints = numOfPoints / workersCount;
	} else {
		divPoints = numOfPoints / workersCount + 1;
		restPoints = numOfPoints - (divPoints * (workersCount - 1));
	}
	for (i = 0; i < workersCount; i++) {
		offsets[i] = tempOffset * sizeof(Point);
		pointsToRead[i] = divPoints;
		if (i == (workersCount - 1)) {
			if (modPoints == 0) {
				pointsToRead[i] = divPoints;
			} else {
				pointsToRead[i] = restPoints;
			}
			break;
		}
		tempOffset += divPoints;

	}
	/* Shell*/
	int retValue, exitProgram = 0;
	int completedCommands = 1;
	char *buf = (char *) malloc(sizeof(char*) * BUFFERSIZE);
	if (buf == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	setbuf(stdout, NULL);
	printf(">>");
	while (exitProgram != 1 && fgets(buf, BUFFERSIZE, stdin) != NULL) {
		retValue = ExecuteCommands(buf, inputBinaryFile, workersCount, tempDir, offsets, pointsToRead,
				completedCommands);
		/* -1 = exit
		 * -2 = wrong command
		 */
		if (retValue == -1) {
			exitProgram = 1;
		} else if (retValue == -2) {
			printf(">>");
			fflush(stdout);
		} else {
			printf("command completed!\nicon generated\n");
			printf(">>");
			fflush(stdout);
			completedCommands++;
		}
	}
	/* Folder Deletion*/
	DIR *dir = opendir(tempDir);
	struct dirent *next_file;
	char filepath[BUFFERSIZE];
	/* Delete files inside the folder*/
	while ((next_file = readdir(dir)) != NULL) {
		/* Build the path for each file in the folder*/
		snprintf(filepath, BUFFERSIZE, "%s/%s", tempDir, next_file->d_name);
		remove(filepath);
	}
	closedir(dir);
	/* Delete the folder*/
	rmdir(tempDir);
	/* Free allocated space*/
	free(buf);
	free(offsets);
	free(pointsToRead);
}

/* Return -1 when the input is "exit;"
 * Return -2 for wrong args
 * exit(1) for system errors
 */
int ExecuteCommands(char *buf, char *inputBinaryFile, int workersCount, char *tempDir, int *offsets, int *pointsToRead,
		int completedCommands) {
	char *cmd;
	char *saveptr;
	char **commands = NULL;
	int numberOfCmds = 0, commandCounter = 0;
	int i = 0;
	/* Check if ';' is missing
	 * If it is missing then cancel the command
	 * If it is present use the string until the first ';'
	 */
	char *buffer = (char *) malloc(sizeof(char*) * (BUFFERSIZE));
	int semicolonFound = 0;
	if (buffer == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Copy the input string until the first ';'*/
	while (buf[i] != '\0') {
		if (buf[i] == ';') {
			buffer[i] = buf[i];
			buffer[i+1] = '\0';
			i++;
			semicolonFound = 1;
			break;
		}
		buffer[i] = buf[i];
		i++;
	}
	if (semicolonFound == 0) {
		printf("';' missing\nevery command must be ending with ';'\n");
		return -2;
	}
	/* Split the entered line to commands*/
	cmd = strtok_r(buffer, ",;", &saveptr);
	while (cmd != NULL) {
		numberOfCmds++;
		commands = realloc(commands, sizeof(char *) * numberOfCmds);
		if (commands == NULL) {
			printf("realloc error %s\n", strerror(errno));
			exit(1);
		}
		commands[commandCounter] = cmd;
		commandCounter++;
		cmd = strtok_r(NULL, ",;\n", &saveptr);
	}
	if (strcmp(commands[0], "exit") == 0) {
		return -1;
	}
	/* Create array to store PIDs of the handlers*/
	int status;
	pid_t waitPID, gnuplotPID;
	pid_t *pids = (pid_t *) malloc(sizeof(pid_t) * (commandCounter));
	if (pids == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Create array to store the colors*/
	char **colors = (char **) malloc(sizeof(char**) * (commandCounter));
	if (colors == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	for (i = 0; i < commandCounter; i++) {
		colors[i] = (char *) malloc(sizeof(char*) * WORDSIZE);
	}
	/* Filter commands for errors*/
	for (i = 0; i < commandCounter; i++) {
		char *token;
		char **words = NULL;
		char *tempChar = (char *) malloc(sizeof(char*) * BUFFERSIZE);
		if (tempChar == NULL) {
			printf("malloc error %s\n", strerror(errno));
			exit(1);
		}
		strncpy(tempChar, commands[i], BUFFERSIZE);
		int numberOfSpaces = 0, tokenCounter = 0;
		/* Split the command into separate words*/
		token = strtok(tempChar, " \t\n");
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
		if (strcmp(words[0], "circle") == 0) {
			if (tokenCounter > 5) {
				printf("too many arguments in circle command\n");
				return -2;
			}
			if (tokenCounter < 5) {
				printf("too few arguments in circle command\n");
				return -2;
			}
			strncpy(colors[i], words[4], WORDSIZE);
		} else if (strcmp(words[0], "semicircle") == 0) {
			if (tokenCounter > 6) {
				printf("too many arguments in semicircle command\n");
				return -2;
			}
			if (tokenCounter < 6) {
				printf("too few arguments in semicircle command\n");
				return -2;
			}
			strncpy(colors[i], words[5], WORDSIZE);
		} else if (strcmp(words[0], "ring") == 0) {
			if (tokenCounter > 6) {
				printf("too many arguments in ring command\n");
				return -2;
			}
			if (tokenCounter < 6) {
				printf("too few arguments in ring command\n");
				return -2;
			}
			strncpy(colors[i], words[5], WORDSIZE);
		} else if (strcmp(words[0], "square") == 0) {
			if (tokenCounter > 5) {
				printf("too many arguments in square command\n");
				return -2;
			}
			if (tokenCounter < 5) {
				printf("too few arguments in square command\n");
				return -2;
			}
			strncpy(colors[i], words[4], WORDSIZE);
		} else if (strcmp(words[0], "ellipse") == 0) {
			if (tokenCounter > 6) {
				printf("too many arguments in ellipse command\n");
				return -2;
			}
			if (tokenCounter < 6) {
				printf("too few arguments in ellipse command\n");
				return -2;
			}
			strncpy(colors[i], words[5], WORDSIZE);
		} else if (strcmp(words[0], "\n") == 0) {
			return 0;
		} else {
			printf("'%s' unknown command\n", words[0]);
			return -2;
		}
		free(tempChar);
		free(token);
		free(words);
	}
	/* Create Handlers*/
	for (i = 0; i < commandCounter; i++) {
		pids[i] = fork();
		if (pids[i] < 0) {
			printf("fork error %s\n", strerror(errno));
			exit(1);
		}
		/* Handler*/
		else if (pids[i] == 0) {
			Handler(commands[i], inputBinaryFile, workersCount, tempDir, offsets, pointsToRead);
		}
	}
	/* WAIT FOR ALL THE HANDLERS */
	while ((waitPID = wait(&status)) > 0) {
	}
	/* Create gnuplot script*/
	char *scriptName = (char *) malloc(sizeof(char*) * BUFFERSIZE);
	if (scriptName == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	snprintf(scriptName, BUFFERSIZE, "%d_script.gnuplot", completedCommands);
	FILE *script = fopen(scriptName, "wb");
	fprintf(script, "set terminal png\nset size ratio -1\nset output \"./%d_image.png\"\nplot ", completedCommands);
	for (i = 0; i < commandCounter; i++) {
		fprintf(script, "\\\n\"%s/%d.out\" notitle with points pointsize 0.5 linecolor rgb \"%s\"", tempDir, pids[i],
				colors[i]);
		if (i < commandCounter - 1) {
			fprintf(script, " ,");
		}
	}
	fflush(script);
	/* Fork and exec* gnuplot*/
	gnuplotPID = fork();
	/* Exec gnuplot*/
	if (gnuplotPID == 0) {
		if (execlp("gnuplot", "gnuplot", scriptName, NULL) < 0) {
			printf("execl error %s\n", strerror(errno));
			exit(1);
		}
		exit(0);
	}
	/* WAIT FOR THE exec**/
	while ((waitPID = wait(&status)) > 0) {
	}
	fclose(script);
	fflush(stdout);
	/* Free allocated space*/
	free(buffer);
	free(commands);
	free(pids);
	for (i = 0; i < commandCounter; i++) {
		free(colors[i]);
	}
	free(colors);
	free(scriptName);
	return 0;
}

