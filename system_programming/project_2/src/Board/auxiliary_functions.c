/*
 * auxiliary_functions.c
 *
 *  Created on: Apr 17, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "auxiliary_functions.h"
#include "definitions.h"

char* Concatenate(char *path, char *boardName, char *filename) {
	char *catstring = malloc(BUFFERSIZE);
	if (catstring == NULL) {
		perror("\nConcatenate: malloc error\nDescription");
		exit(1);
	}
	strncpy(catstring, path, BUFFERSIZE);
	strncat(catstring, "/", BUFFERSIZE);
	strncat(catstring, boardName, BUFFERSIZE);
	strncat(catstring, filename, BUFFERSIZE);
	return catstring;
}
