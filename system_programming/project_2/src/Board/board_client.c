/*
 * board_client.c
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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "board_client.h"
#include "auxiliary_functions.h"
#include "definitions.h"

void BoardClientShell(char *path, char *boardName) {
	// Ignore Ctrl+C
	signal(SIGINT, SIG_IGN);
	printf("########################\n");
	printf("      BOARD CLIENT\n");
	printf("########################\n");
	int stopClient = 0;
	char buf[BUFFERSIZE];
	printf(">>");
	while (stopClient != 1 && fgets(buf, BUFFERSIZE, stdin) != NULL) {
		if (BoardClientCommands(buf, path, boardName) == 1) {
			stopClient = 1;
		} else {
			printf(">>");
		}
	}
	printf("\n");
	return;
}

int BoardClientCommands(char *lineBuffer, char *path, char *boardName) {
	char* token;
	char** words = NULL;
	int numberOfSpaces = 0, tokencounter = 0;
	//Split the entered line to words
	token = strtok(lineBuffer, " \t");
	while (token != NULL) {
		numberOfSpaces++;
		words = realloc(words, sizeof(char*) * numberOfSpaces);
		if (words == NULL) {
			perror("\nBoardClient: malloc error\nDescription");
			exit(1);
		}
		words[tokencounter] = token;
		tokencounter++;
		token = strtok(NULL, " \t\n");
	}
	//Execute given command
	int id;
	char *name = malloc(BUFFERSIZE);
	if (name == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	if (strcmp(words[0], "createchannel") == 0) {
		if (tokencounter <= 2) {
			printf("\ntoo few arguments!\n");
			return 3;
		}
		if (tokencounter > 3) {
			printf("\ntoo many arguments!\n");
			return 3;
		}
		id = atoi(words[1]);
		strcpy(name, words[2]);
		BoardClientCreatechannel(path, id, name, boardName);
	} else if (strcmp(words[0], "getmessages") == 0) {
		if (tokencounter <= 1) {
			printf("\ntoo few arguments!\n");
			return 3;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return 3;
		}
		id = atoi(words[1]);
		BoardClientGetMessages(path, id, boardName);
	} else if (strcmp(words[0], "exit\n") == 0) {
		if (tokencounter > 1) {
			printf("\ntoo many arguments!\n");
			return 3;

		}
		printf("\nclient exiting\n");
		return 1;
	} else if (strcmp(words[0], "shutdown\n") == 0) {
		if (tokencounter > 1) {
			printf("\ntoo many arguments!\n");
			return 3;
		}
		BoardClientShutdown(path, boardName);
		printf("client exiting\n");
		return 1;
	} else {
		printf("\nunknown command\n");
	}
	free(token);
	free(words);
	return 0;
}

void BoardClientCreatechannel(char *path, int id, char *name, char *boardName) {
	int client_server, writeClientToServer;
	int server_client, readServerToClient;
	char createmsg[MESSAGESIZE] = "CRE ";
	char charId[10];
	snprintf(charId, 10, "%d", id);
	strncat(createmsg, charId, MESSAGESIZE);
	strncat(createmsg, " ", MESSAGESIZE);
	strncat(createmsg, name, MESSAGESIZE);
	strncat(createmsg, " /", MESSAGESIZE);
	// Open Server_Client_fifo
	char *serverToClient = malloc(BUFFERSIZE);
	if (serverToClient == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	serverToClient = Concatenate(path, boardName, "_Server_Client_fifo");
	if ((server_client = open(serverToClient, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardClient: fifo open error\nDescription");
		exit(3);
	}
	// Open Client_Server_fifo
	char *clientToServer = malloc(BUFFERSIZE);
	if (clientToServer == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	clientToServer = Concatenate(path, boardName, "_Client_Server_fifo");
	if ((client_server = open(clientToServer, O_WRONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardClient: fifo open error\nDescription");
		exit(3);
	}
	// Send "CRE %d(ch_id) %s(ch_name) /" to server through Client_Server_fifo
	if ((writeClientToServer = write(client_server, createmsg, MESSAGESIZE)) == -1) {
		perror("\nBoardClient: fifo write error\nDescription");
		exit(2);
	}
	usleep(100000);
	// Read from Server_Client_fifo
	char msgFromServer[MESSAGESIZE];
	readServerToClient = read(server_client, msgFromServer, MESSAGESIZE);
	if (readServerToClient < 0) {
		perror("\nBoardClient: fifo read error\nDescription");
		exit(4);
	}
	if (strncmp(msgFromServer, "OK", 2) == 0) {
		printf("\nchannel with id: %d and name: %s created\n", id, name);
	} else if (strncmp(msgFromServer, "CH_IS_CREATED", 13) == 0) {
		printf("\nchannel is already created\n");
	}
	free(clientToServer);
	free(serverToClient);
	close(server_client);
	close(client_server);
}

void BoardClientGetMessages(char *path, int id, char *boardName) {
	int client_server, writeClientToServer;
	int server_client, readServerToClient;
	char getmsg[MESSAGESIZE] = "GET ";
	char charId[10];
	snprintf(charId, 10, "%d", id);
	strncat(getmsg, charId, MESSAGESIZE);
	strncat(getmsg, " /", MESSAGESIZE);
	// Open Server_Client_fifo
	char *serverToClient = malloc(BUFFERSIZE);
	if (serverToClient == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	serverToClient = Concatenate(path, boardName, "_Server_Client_fifo");
	if ((server_client = open(serverToClient, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardClient: fifo open error\nDescription");
		exit(3);
	}
	// Open Client_Server_fifo
	char *clientToServer = malloc(BUFFERSIZE);
	if (clientToServer == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	clientToServer = Concatenate(path, boardName, "_Client_Server_fifo");
	if ((client_server = open(clientToServer, O_WRONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardClient: fifo open error\nDescription");
		exit(3);
	}
	// Send "GET %d(ch_id) /" to server through Client_Server_fifo
	if ((writeClientToServer = write(client_server, getmsg, MESSAGESIZE)) == -1) {
		perror("\nBoardClient: fifo write error\nDescription");
		exit(2);
	}
	usleep(100000);
	// Read from Server_Client_fifo
	char msgFromServer[MESSAGESIZE];
	int flagToStopReading = 0;
	while (flagToStopReading == 0) {
		readServerToClient = read(server_client, msgFromServer, MESSAGESIZE);
		if (readServerToClient < 0) {
			perror("\nBoardClient: fifo read error\nDescription");
			flagToStopReading = 1;
		} else if (readServerToClient > 0) {
			// No messages in this channel
			if (strncmp(msgFromServer, "NO_MSG", 6) == 0) {
				flagToStopReading = 1;
				printf("\nno messages in this channel\n");
			}
			// Channel does not exist
			else if (strncmp(msgFromServer, "NO_CH", 5) == 0) {
				flagToStopReading = 1;
				printf("\nthe channel you entered does not exist\n");
			}
			// End of messages
			else if (strncmp(msgFromServer, "END", 3) == 0) {
				flagToStopReading = 1;
			}
			// Server sends files
			else if (strncmp(msgFromServer, "FLE", 3) == 0) {
				// Get the offset of the received message (the message will be like "FLE file1 \")
				int i = 0;
				char *offset = malloc(BUFFERSIZE);
				while (*(msgFromServer + i) != '/') {
					offset[i] = msgFromServer[i];
					i++;
				}
				offset[i + 1] = '\0';
				char* token;
				char** words = NULL;
				int numberOfSpaces = 0, tokencounter = 0;
				// Split the offset
				token = strtok(offset, " ");
				while (token != NULL) {
					numberOfSpaces++;
					words = realloc(words, sizeof(char*) * numberOfSpaces);
					if (words == NULL) {
						perror("\nBoardClient: malloc error\nDescription");
						exit(1);
					}
					words[tokencounter] = token;
					tokencounter++;
					token = strtok(NULL, " ");
				}
				char fileName[BUFFERSIZE];
				strncpy(fileName, words[1], BUFFERSIZE);
				char directory[BUFFERSIZE];
				strncpy(directory, boardName, BUFFERSIZE);
				strncat(directory, " Files", BUFFERSIZE);
				int err = mkdir(directory, 0700);
				// Create a new directory for the new board
				if (err == 0) {
					printf("directory %s just created\n", directory);
				}
				if (err < 0) {
					if (errno != EEXIST) {
						perror("\nBoardClient: mkdir error\nDescription");
						exit(2);
					}
				}
				char channelDir[BUFFERSIZE];
				strncpy(channelDir, directory, BUFFERSIZE);
				strncat(channelDir, "/", BUFFERSIZE);
				strncat(channelDir, charId, BUFFERSIZE);
				err = mkdir(channelDir, 0700);
				// Create a new directory for the new board
				if (err == 0) {
					printf("directory %s just created\nall the files of this channel will be stored there\n", channelDir);
				}
				if (err < 0) {
					if (errno != EEXIST) {
						perror("\nBoardClient: mkdir error\nDescription");
						exit(2);
					}
				}
				char pathToFile[BUFFERSIZE];
				strncpy(pathToFile, channelDir, BUFFERSIZE);
				strncat(pathToFile, "/", BUFFERSIZE);
				strncat(pathToFile, fileName, BUFFERSIZE);
				FILE *fd = fopen(pathToFile, "w");
				char endOfFile = 0;
				char fileLine[MESSAGESIZE];
				while (endOfFile == 0) {
					readServerToClient = read(server_client, fileLine, MESSAGESIZE);
					if (strncmp(fileLine, "EOF", 3) != 0) {
						fprintf(fd, "%s\n", fileLine);
						fflush(fd);
					} else {
						endOfFile = 1;
					}
				}
				printf("\nfile received\n");
				free(offset);
				free(words);
				free(token);
			} else {
				// Print message
				printf("%s\n", msgFromServer);
			}
		}
	}
	usleep(100000);
	close(client_server);
	close(server_client);
	free(clientToServer);
}

void BoardClientShutdown(char* path, char *boardName) {
	int client_server, writeClientToServer;
	int server_client;
	char shutdwnmsg[MESSAGESIZE] = "SHUTDWN";
	strncat(shutdwnmsg, " /", MESSAGESIZE);
	// Open Server_Client_fifo
	char *serverToClient = malloc(BUFFERSIZE);
	if (serverToClient == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	serverToClient = Concatenate(path, boardName, "_Server_Client_fifo");
	if ((server_client = open(serverToClient, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardClient: fifo open error\nDescription");
		exit(3);
	}
	// Open Client_Server_fifo
	char *clientToServer = malloc(BUFFERSIZE);
	if (clientToServer == NULL) {
		perror("\nBoardClient: malloc error\nDescription");
		exit(1);
	}
	clientToServer = Concatenate(path, boardName, "_Client_Server_fifo");
	if ((client_server = open(clientToServer, O_WRONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardClient: fifo open error\nDescription");
		exit(3);
	}
	// Send "SHUTDWN /" to server through Client_Server_fifo
	if ((writeClientToServer = write(client_server, shutdwnmsg, MESSAGESIZE)) == -1) {
		perror("\nBoardClient: fifo write error\nDescription");
		exit(2);
	}
	usleep(100000);
	printf("\nserver is shutting down\n");
	printf("deleting board\n");
	free(clientToServer);
	free(serverToClient);
	return;
}
