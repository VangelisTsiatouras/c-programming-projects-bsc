/*
 * main.c
 *
 *  Created on: Apr 17, 2016
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
#include <sys/stat.h>
#include "board_post.h"

int main(int argc, char *argv[]) {
	if (argc > 2) {
		printf("too many arguments!\n");
		exit(1);
	} else if (argc == 1) {
		printf("too few arguments!\n");
		exit(1);
	}
	DIR *boardDir;
	if ((boardDir = opendir(argv[1])) == NULL) {
		perror("\nBoardPost: opendir error\nDescription");
		exit(1);
	} else {
		printf("\ndirectory opened\n");
	}
	// Name of the board
	char *boardName = basename(argv[1]);
	printf("board name: %s\n", boardName);
	BoardPostShell(argv[1], boardName);
	exit(0);
}

