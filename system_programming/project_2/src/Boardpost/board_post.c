/*
 * board_post.c
 *
 *  Created on: Apr 18, 2016
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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "board_post.h"

#define BUFFERSIZE 256
#define MESSAGESIZE 2048

void BoardPostShell(char *path, char *boardName) {
	printf("########################\n");
	printf("       BOARD POST\n");
	printf("########################\n");
	int stopBPost = 0;
	char buf[BUFFERSIZE];
	printf(">>");
	while (stopBPost != 1 && fgets(buf, BUFFERSIZE, stdin) != NULL) {
		if (BoardPostCommands(buf, path, boardName) == 1) {
			stopBPost = 1;
		} else {
			printf(">>");
		}
	}
	printf("\n");
	return;
}

int BoardPostCommands(char *lineBuffer, char *path, char *boardName) {
	char* token;
	char** words = NULL;
	int numberOfSpaces = 0, tokencounter = 0;
	//Split the entered line to words
	token = strtok(lineBuffer, " \t");
	while (token != NULL) {
		numberOfSpaces++;
		words = realloc(words, sizeof(char*) * numberOfSpaces);
		if (words == NULL) {
			perror("\nBoardPost: malloc error\nDescription");
			exit(1);
		}
		words[tokencounter] = token;
		tokencounter++;
		token = strtok(NULL, " \t\n");
	}
	//Execute given command
	int id;
	char *input = malloc(MESSAGESIZE);
	if (input == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	if (strcmp(words[0], "list\n") == 0) {
		if (tokencounter > 1) {
			printf("\ntoo many arguments!\n");
			return 2;
		}
		// Call list
		BoardPostList(path, boardName);
	} else if (strcmp(words[0], "write") == 0) {
		if (tokencounter <= 2) {
			printf("\ntoo few arguments!\n");
			return 2;
		}
		if (tokencounter > 3) {
			printf("\ntoo many arguments!\n");
			return 2;
		}
		// Call write
		id = atoi(words[1]);
		strncpy(input, words[2], MESSAGESIZE);
		BoardPostWrite(id, input, path, boardName);
	} else if (strcmp(words[0], "send") == 0) {
		if (tokencounter <= 2) {
			printf("\ntoo few arguments!\n");
			return 2;
		}
		if (tokencounter > 3) {
			printf("\ntoo many arguments!\n");
			return 2;
		}
		// Call send
		id = atoi(words[1]);
		strncpy(input, words[2], BUFFERSIZE);
		BoardPostSend(id, input, path, boardName);
	} else if (strcmp(words[0], "exit\n") == 0) {
		if (tokencounter > 1) {
			printf("\ntoo many arguments!\n");
			return 2;
		}
		printf("\nboard post exiting\n");
		return 1;
	} else {
		printf("\nunknown command\n");
	}
	free(token);
	free(words);
	return 0;
}

void BoardPostList(char *path, char *boardName) {
	int bpost_server, server_bpost, writeBPostToServer, readServerToBPost;
	char listmsg[MESSAGESIZE] = "LST ";
	strncat(listmsg, " /", MESSAGESIZE);
	// Open Server_BoardPost_fifo
	char *serverToBoardPost = malloc(BUFFERSIZE);
	if (serverToBoardPost == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	serverToBoardPost = Concatenate(path, boardName, "_Server_BoardPost_fifo");
	if ((server_bpost = open(serverToBoardPost, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardPost: fifo open error\nDescription");
		exit(3);
	}
	// Open BoardPost_Server_fifo
	char *boardPostToServer = malloc(BUFFERSIZE);
	if (boardPostToServer == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	boardPostToServer = Concatenate(path, boardName, "_BoardPost_Server_fifo");
	if ((bpost_server = open(boardPostToServer, O_WRONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardPost: fifo open error\nDescription");
		exit(3);
	}
	// Send "LST /" to server through BoardPost_Server_fifo
	if ((writeBPostToServer = write(bpost_server, listmsg, MESSAGESIZE + 1)) == -1) {
		perror("\nBoardPost: fifo write error\nDescription");
		exit(2);
	}
	usleep(100000);
	// Receive feedback from server through Server_BoardPost_fifo
	char feedback[MESSAGESIZE];
	readServerToBPost = read(server_bpost, feedback, MESSAGESIZE);
	if (serverToBoardPost < 0) {
		perror("\nBoardPost: fifo read error\nDescription");
		exit(5);
	}
	if (strncmp(feedback, "NO_CH", 5) == 0) {
		printf("\nno active channels\n");
	} else {
		printf("\n%s\n", feedback);
	}
	free(boardPostToServer);
	free(serverToBoardPost);
	close(bpost_server);
	close(server_bpost);
}

void BoardPostWrite(int id, char *message, char *path, char *boardName) {
	int bpost_server, server_bpost, writeBPostToServer, readServerToBPost;
	char sendmsg[MESSAGESIZE] = "MSG ";
	char charId[10];
	snprintf(charId, 10, "%d", id);
	strncat(sendmsg, charId, MESSAGESIZE);
	strncat(sendmsg, " / ", MESSAGESIZE);
	strncat(sendmsg, message, MESSAGESIZE);
	// THIS SHOULD NEVER HAPPEN
	if (strlen(sendmsg) > MESSAGESIZE) {
		printf("\nmessage is too long\n");
		fflush(stdout);
		return;
	}
	// Open Server_BoardPost_fifo
	char *serverToBoardPost = malloc(BUFFERSIZE);
	if (serverToBoardPost == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	serverToBoardPost = Concatenate(path, boardName, "_Server_BoardPost_fifo");
	if ((server_bpost = open(serverToBoardPost, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardPost: fifo open error\nDescription");
		exit(3);
	}
	// Open BoardPost_Server_fifo
	char *boardPostToServer = malloc(BUFFERSIZE);
	if (boardPostToServer == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	boardPostToServer = Concatenate(path, boardName, "_BoardPost_Server_fifo");
	if ((bpost_server = open(boardPostToServer, O_WRONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardPost: fifo open error\nDescription");
		exit(3);
	}
	// Send "MSG %d(ch_id) %s(message) /" to server through BoardPost_Server_fifo
	if ((writeBPostToServer = write(bpost_server, sendmsg, MESSAGESIZE + 1)) == -1) {
		perror("\nBoardPost: fifo write error\nDescription");
		exit(2);
	}
	usleep(100000);
	// Receive feedback from server through Server_BoardPost_fifo
	char feedback[MESSAGESIZE];
	readServerToBPost = read(server_bpost, feedback, MESSAGESIZE);
	if (serverToBoardPost < 0) {
		perror("\nBoardPost: fifo read error\nDescription");
		exit(5);
	}
	if (strcmp(feedback, "OK") == 0) {
		printf("\nmessage sent\n");
	} else if (strcmp(feedback, "NO_CH") == 0) {
		printf("\nthe channel you entered does not exist\n");
	}
	free(serverToBoardPost);
	free(boardPostToServer);
	close(server_bpost);
	close(bpost_server);
}

void BoardPostSend(int id, char *pathOfFile, char *path, char *boardName) {
	FILE *f = fopen(pathOfFile, "r");
	if (f == NULL) {
		printf("file not found!\n");
		return;
	}
	char *fileName = basename(pathOfFile);
	printf("filename %s\n", fileName);
	int bpost_server, server_bpost, writeBPostToServer, readServerToBPost;
	// Open Server_BoardPost_fifo
	char *serverToBoardPost = malloc(BUFFERSIZE);
	if (serverToBoardPost == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	serverToBoardPost = Concatenate(path, boardName, "_Server_BoardPost_fifo");
	if ((server_bpost = open(serverToBoardPost, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardPost: fifo open error\nDescription");
		exit(3);
	}
	// Open BoardPost_Server_fifo
	char *boardPostToServer = malloc(BUFFERSIZE);
	if (boardPostToServer == NULL) {
		perror("\nBoardPost: malloc error\nDescription");
		exit(1);
	}
	boardPostToServer = Concatenate(path, boardName, "_BoardPost_Server_fifo");
	if ((bpost_server = open(boardPostToServer, O_WRONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardPost: fifo open error\nDescription");
		exit(3);
	}
	// Send "FLE %d(ch_id) %s(name_of_File) /" to server through BoardPost_Server_fifo
	char sendmsg[MESSAGESIZE] = "FLE ";
	char charId[10];
	snprintf(charId, 10, "%d", id);
	strncat(sendmsg, charId, MESSAGESIZE);
	strncat(sendmsg, " ", MESSAGESIZE);
	strncat(sendmsg, fileName, MESSAGESIZE);
	strncat(sendmsg, " ", MESSAGESIZE);
	strncat(sendmsg, " /", MESSAGESIZE);
	if ((writeBPostToServer = write(bpost_server, sendmsg, MESSAGESIZE)) == -1) {
		perror("\nBoardPost: fifo write error\nDescription");
		exit(2);
	}
	usleep(100000);
	// Receive feedback from server through Server_BoardPost_fifo
	char feedback[MESSAGESIZE];
	int serverReadyForUpload = 0;
	readServerToBPost = read(server_bpost, feedback, MESSAGESIZE);
	if (serverToBoardPost < 0) {
		perror("\nBoardPost: fifo read error\nDescription");
		exit(5);
	} else if (readServerToBPost > 0) {
		if (strcmp(feedback, "STARTSEND") == 0) {
			serverReadyForUpload = 1;
		} else if (strcmp(feedback, "NO_CH") == 0) {
			serverReadyForUpload = 0;
			printf("\nthe channel you entered does not exist\n");
		}
	}
	// Ready to upload the file line by line to the server
	if (serverReadyForUpload == 1) {
		while (fgets(sendmsg, MESSAGESIZE, f) != NULL) {
			sendmsg[strlen(sendmsg) - 1] = '\0';
			//printf("%s\n", sendmsg);
			if ((writeBPostToServer = write(bpost_server, sendmsg, MESSAGESIZE)) == -1) {
				perror("\nBoardPost: fifo write error\nDescription");
				exit(2);
			}
		}
		strncpy(sendmsg, "END", 3);
		if ((writeBPostToServer = write(bpost_server, sendmsg, MESSAGESIZE)) == -1) {
			perror("\nBoardPost: fifo write error\nDescription");
			exit(2);
		}
		printf("\nfile sent\n");
	}
	free(serverToBoardPost);
	free(boardPostToServer);
	close(server_bpost);
	close(bpost_server);
}

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
