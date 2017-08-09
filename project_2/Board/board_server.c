/*
 * board_server.c
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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "board_server.h"
#include "board_channels.h"
#include "auxiliary_functions.h"
#include "definitions.h"

void BoardServer(char *path, char *boardName) {
	// Ignore Ctrl+C
	signal(SIGINT, SIG_IGN);
	char *pathToServerPID = malloc(BUFFERSIZE);
	if (pathToServerPID == NULL) {
		perror("\nBoardServer: malloc error\nDescription");
		exit(1);
	}
	FILE *serverPID;
	pid_t pid;
	pid = getpid();
	// Write your pid to server_PID.txt
	pathToServerPID = Concatenate(path, boardName, "_Server_PID.txt");
	serverPID = fopen(pathToServerPID, "w+");
	fprintf(serverPID, "%d", pid);
	fclose(serverPID);
	free(pathToServerPID);
	// Create the FIFOs
	char *boardPostToServer = malloc(BUFFERSIZE);
	if (boardPostToServer == NULL) {
		perror("\nBoardServer: malloc error\nDescription");
		exit(1);
	}
	char *serverToBoardPost = malloc(BUFFERSIZE);
	if (serverToBoardPost == NULL) {
		perror("\nBoardServer: malloc error\nDescription");
		exit(1);
	}
	char *clientToServer = malloc(BUFFERSIZE);
	if (clientToServer == NULL) {
		perror("\nBoardServer: malloc error\nDescription");
		exit(1);
	}
	char *serverToClient = malloc(BUFFERSIZE);
	if (serverToClient == NULL) {
		perror("\nBoardServer: malloc error\nDescription");
		exit(1);
	}
	// boardPostToServer FIFO
	boardPostToServer = Concatenate(path, boardName, "_BoardPost_Server_fifo");
	if (mkfifo(boardPostToServer, 0666) == -1) {
		if (errno != EEXIST) {
			perror("\nBoardServer: mkfifo error\nDescription");
			exit(2);
		}
	}
	// serverToBoardPost FIFO
	serverToBoardPost = Concatenate(path, boardName, "_Server_BoardPost_fifo");
	if (mkfifo(serverToBoardPost, 0666) == -1) {
		if (errno != EEXIST) {
			perror("\nBoardServer: mkfifo error\nDescription");
			exit(2);
		}
	}
	// clientToServer FIFO
	clientToServer = Concatenate(path, boardName, "_Client_Server_fifo");
	if (mkfifo(clientToServer, 0666) == -1) {
		if (errno != EEXIST) {
			perror("\nBoardServer: mkfifo error\nDescription");
			exit(2);
		}
	}
	// serverToClient FIFO
	serverToClient = Concatenate(path, boardName, "_Server_Client_fifo");
	if (mkfifo(serverToClient, 0666) == -1) {
		if (errno != EEXIST) {
			perror("\nBoardServer: mkfifo error\nDescription");
			exit(2);
		}
	}
	char msgFromClient[MESSAGESIZE];
	char msgFromBPost[MESSAGESIZE];
	int bpost_server, client_server;
	int readClientServer, readBPostServer;
	// Open read only FIFOs(boardpost->server, client->server)
	if ((bpost_server = open(boardPostToServer, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardServer: fifo open error\nDescription");
		exit(3);
	}
	if ((client_server = open(clientToServer, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("\nBoardServer: fifo open error\nDescription");
		exit(3);
	}
	// Initialize the data structure
	int i, j;
	BoardChannels savedData;
	savedData.maxActiveChannels = 5;
	savedData.numberOfActiveChannels = 0;
	savedData.channelArray = malloc(sizeof(Channel) * savedData.maxActiveChannels);
	for (i = 0; i < savedData.maxActiveChannels; i++) {
		savedData.channelArray[i].id = -1;
		savedData.channelArray[i].maxFiles = 5;
		savedData.channelArray[i].numberOfFiles = 0;
		savedData.channelArray[i].filesFDs = malloc(sizeof(FILE*) * 5);
		savedData.channelArray[i].maxMessages = 5;
		savedData.channelArray[i].numberOfMessages = 0;
		savedData.channelArray[i].messages = malloc(sizeof(char*) * 5);
		if (savedData.channelArray[i].messages == NULL) {
			perror("\nBoardServer: malloc error\nDescription");
			exit(1);
		}
		for (j = 0; j < savedData.channelArray[i].maxMessages; j++) {
			savedData.channelArray[i].messages[j] = malloc(sizeof(char) * MESSAGESIZE);
			if (savedData.channelArray[i].messages[j] == NULL) {
				perror("\nBoardServer: malloc error\nDescription");
				exit(1);
			}
			strcpy(savedData.channelArray[i].messages[j], "NOMSG");
		}
	}
	while (1) {
		// Read from Client_Server_fifo
		readClientServer = read(client_server, msgFromClient, MESSAGESIZE + 1);
		if (readClientServer < 0) {
			perror("\nBoardServer: fifo read error\nDescription");
			exit(5);
		} else if (readClientServer > 0) {
			// Get the offset of the received message
			int i = 0;
			char *offset = malloc(BUFFERSIZE);
			while (*(msgFromClient + i) != '/') {
				offset[i] = msgFromClient[i];
				i++;
			}
			offset[i + 1] = '\0';
			// Split the offset to words
			char* token;
			char** words = NULL;
			int numberOfSpaces = 0, tokencounter = 0;
			token = strtok(offset, " ");
			while (token != NULL) {
				numberOfSpaces++;
				words = realloc(words, sizeof(char*) * numberOfSpaces);
				if (words == NULL) {
					perror("\nBoardServer: malloc error\nDescription");
					exit(1);
				}
				words[tokencounter] = token;
				tokencounter++;
				token = strtok(NULL, " ");
			}
			int server_client;
			// Open Server_Client_fifo
			if ((server_client = open(serverToClient, O_WRONLY | O_NONBLOCK)) < 0) {
				perror("\nBoardServer: fifo open error\nDescription");
				exit(3);
			}
			// Create new channel
			// words[0] == CRE, words[1] == id, words[2] == name
			if (strcmp(words[0], "CRE") == 0) {
				// Check if there is already the channel
				int chId = atoi(words[1]);
				int chFound = 0;
				int iter1 = 0;
				while (iter1 < savedData.maxActiveChannels && chFound == 0) {
					if (savedData.channelArray[iter1].id == chId) {
						chFound = 1;
					}
					iter1++;
				}
				char feedbackCreate[MESSAGESIZE];
				// Channel exists
				if (chFound == 1) {
					strncpy(feedbackCreate, "CH_IS_CREATED", MESSAGESIZE);
					int writeServerToClient;
					if ((writeServerToClient = write(server_client, feedbackCreate, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
					usleep(100000);
				}
				// Channel does not exist
				else {
					int stored = 0;
					int iter2;
					// Check if there is available space for new channel, if not then reallocate space...
					if (savedData.maxActiveChannels == savedData.numberOfActiveChannels) {
						int oldSize = savedData.maxActiveChannels;
						int doubleSize = savedData.maxActiveChannels * 2;
						savedData.channelArray = realloc(savedData.channelArray, sizeof(Channel) * doubleSize);
						if (savedData.channelArray == NULL) {
							perror("\nBoardServer: realloc error\nDescription");
							exit(1);
						}
						// Reinitialize
						savedData.maxActiveChannels = doubleSize;
						int j;
						for (iter2 = 0; iter2 < savedData.maxActiveChannels; iter2++) {
							if (iter2 >= oldSize) {
								savedData.channelArray[iter2].id = -1;
								savedData.channelArray[iter2].maxFiles = 5;
								savedData.channelArray[iter2].numberOfFiles = 0;
								savedData.channelArray[iter2].filesFDs = malloc(sizeof(FILE*) * 5);
								savedData.channelArray[iter2].maxMessages = 5;
								savedData.channelArray[iter2].numberOfMessages = 0;
								savedData.channelArray[iter2].messages = malloc(sizeof(char*) * 5);
								if (savedData.channelArray[iter2].messages == NULL) {
									perror("\nBoardServer: malloc error\nDescription");
									exit(1);
								}
								for (j = 0; j < savedData.channelArray[i].maxMessages; j++) {
									savedData.channelArray[i].messages[j] = malloc(sizeof(char) * MESSAGESIZE);
									if (savedData.channelArray[i].messages[j] == NULL) {
										perror("\nBoardServer: malloc error\nDescription");
										exit(1);
									}
									strcpy(savedData.channelArray[i].messages[j], "NOMSG");
								}
							}
						}
					}
					// Create new channel
					iter2 = 0;
					while (iter2 < savedData.maxActiveChannels && stored == 0) {
						if (savedData.channelArray[iter2].id == -1) {
							savedData.channelArray[iter2].id = atoi(words[1]);
							strncpy(savedData.channelArray[iter2].name, words[2], 64);
							savedData.numberOfActiveChannels++;
							stored = 1;
						}
						iter2++;
					}
				}
				strncpy(feedbackCreate, "OK", MESSAGESIZE);
				int writeServerToClient;
				if ((writeServerToClient = write(server_client, feedbackCreate, MESSAGESIZE)) == -1) {
					perror("\nBoardServer: fifo write error\nDescription");
					exit(2);
				}
				usleep(100000);
			}
			// Get messages
			// words[0] == GET, words[1] == id
			else if (strcmp(words[0], "GET") == 0) {
				char feedbackGetMessages[MESSAGESIZE];
				// Check if channel exists
				int chId = atoi(words[1]);
				int chFound = 0, iter1 = 0;
				int positionOfChannelInArray;
				while (iter1 < savedData.maxActiveChannels && chFound == 0) {
					if (savedData.channelArray[iter1].id == chId) {
						positionOfChannelInArray = iter1;
						chFound = 1;
					}
					iter1++;
				}
				// If channel exists
				if (chFound == 1) {
					int messageFound = 0;
					// Check if there are saved messages
					if (savedData.channelArray[positionOfChannelInArray].numberOfMessages > 0) {
						int iter2;
						for (iter2 = 0; iter2 < savedData.channelArray[positionOfChannelInArray].maxMessages; iter2++) {
							if (strcmp(savedData.channelArray[positionOfChannelInArray].messages[iter2], "NOMSG")
									!= 0) {
								messageFound = 1;
								strncpy(feedbackGetMessages,
										savedData.channelArray[positionOfChannelInArray].messages[iter2],
										MESSAGESIZE);
								// Write messages one by one in the  Server_Client_fifo
								int writeServerToClient;
								if ((writeServerToClient = write(server_client, feedbackGetMessages, MESSAGESIZE))
										== -1) {
									perror("\nBoardServer: fifo write error\nDescription");
									exit(2);
								}
								strcpy(savedData.channelArray[positionOfChannelInArray].messages[iter2], "NOMSG");
								savedData.channelArray[positionOfChannelInArray].numberOfMessages--;
							}
						}
					}
					// Check if there are saved files
					if (savedData.channelArray[positionOfChannelInArray].numberOfFiles > 0) {
						int iter2;
						for (iter2 = 0; iter2 < savedData.channelArray[positionOfChannelInArray].maxFiles; iter2++) {
							if (savedData.channelArray[positionOfChannelInArray].filesFDs[iter2] != NULL) {
								messageFound = 1;
								char numberOfFile[10];
								snprintf(numberOfFile, 10, "%d", iter2);
								char filename[BUFFERSIZE] = "file";
								strcat(filename, numberOfFile);
								// Send "FLE %s(name_of_File) /" to server through Server_Client_fifo
								char sendfiles[MESSAGESIZE] = "FLE ";
								strncat(sendfiles, filename, MESSAGESIZE);
								strncat(sendfiles, " ", MESSAGESIZE);
								strncat(sendfiles, " /", MESSAGESIZE);
								int writeServerToClient;
								if ((writeServerToClient = write(server_client, sendfiles, MESSAGESIZE)) == -1) {
									perror("\nBoardServer: fifo write error\nDescription");
									exit(2);
								}
								// Send line by line the file
								while (fgets(sendfiles, MESSAGESIZE,
										savedData.channelArray[positionOfChannelInArray].filesFDs[iter2]) != NULL) {
									sendfiles[strlen(sendfiles) - 1] = '\0';
									if ((writeServerToClient = write(server_client, sendfiles, MESSAGESIZE)) == -1) {
										perror("\nBoardServer: fifo write error\nDescription");
										exit(2);
									}
								}
								strncpy(sendfiles, "EOF", 3);
								if ((writeServerToClient = write(server_client, sendfiles, MESSAGESIZE)) == -1) {
									perror("\nBoardServer: fifo write error\nDescription");
									exit(2);
								}
								savedData.channelArray[positionOfChannelInArray].numberOfFiles--;
								fclose(savedData.channelArray[positionOfChannelInArray].filesFDs[iter2]);
								savedData.channelArray[positionOfChannelInArray].filesFDs[iter2] = NULL;
							}
						}
					}
					// If there are not saved messages or files send NO_MSG
					if (messageFound == 0) {
						strncpy(feedbackGetMessages, "NO_MSG", MESSAGESIZE);
						int writeServerToClient;
						if ((writeServerToClient = write(server_client, feedbackGetMessages, MESSAGESIZE)) == -1) {
							perror("\nBoardServer: fifo write error\nDescription");
							exit(2);
						}
					}
					// If the server send messages, send also "END" message to client
					else {
						strncpy(feedbackGetMessages, "END", MESSAGESIZE);
						int writeServerToClient;
						if ((writeServerToClient = write(server_client, feedbackGetMessages, MESSAGESIZE)) == -1) {
							perror("\nBoardServer: fifo write error\nDescription");
							exit(2);
						}
					}
				}
				// If channel does not exist send NO_CH
				else {
					strncpy(feedbackGetMessages, "NO_CH", MESSAGESIZE);
					int writeServerToClient;
					if ((writeServerToClient = write(server_client, feedbackGetMessages, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
				}
			}
			// Shutdown
			// words[0] == SHUTDWN
			else if (strcmp(words[0], "SHUTDWN") == 0) {
				// Free memory from the data struct
				int k;
				for (k = 0; k < savedData.maxActiveChannels; k++) {
					int z;
					for (z = 0; z < savedData.channelArray[k].maxMessages; z++) {
						free(savedData.channelArray[k].messages[z]);
					}
					free(savedData.channelArray[k].filesFDs);
					free(savedData.channelArray[k].messages);
				}
				free(savedData.channelArray);
				// Remove (board_name)_serverPID.txt
				char *pathToServerPID = malloc(BUFFERSIZE);
				if (pathToServerPID == NULL) {
					perror("BoardClient: malloc error\nDescription");
					exit(1);
				}
				pathToServerPID = Concatenate(path, boardName, "_Server_PID.txt");
				if (remove(pathToServerPID) != 0) {
					perror("\nBoardClient: remove error\nDescription");
					exit(1);
				}
				free(pathToServerPID);
				// Unlink FIFOs
				// boardPostToServer FIFO
				if (unlink(boardPostToServer) == -1) {
					if (errno != EEXIST) {
						perror("\nBoardClient: unlink error\nDescription");
						exit(1);
					}
				}
				// serverToBoardPost FIFO
				if (unlink(serverToBoardPost) == -1) {
					if (errno != EEXIST) {
						perror("BoardClient: unlink error\nDescription");
						exit(1);
					}
				}
				// clientToServer FIFO;
				if (unlink(clientToServer) == -1) {
					if (errno != EEXIST) {
						perror("\nBoardClient: unlink error\nDescription");
						exit(1);
					}
				}
				// serverToClient FIFO
				if (unlink(serverToClient) == -1) {
					if (errno != EEXIST) {
						perror("\nBoardClient: unlink error\nDescription");
						exit(1);
					}
				}
				// Delete the directory of the board
				if (rmdir(path) != 0) {
					perror("\nBoardClient: remove dir error\nDescription");
					exit(1);
				}
				free(serverToClient);
				free(boardPostToServer);
				free(serverToBoardPost);
				free(clientToServer);
				exit(0);
			}
			usleep(100000);
			close(server_client);
			free(offset);
			free(words);
			free(token);
		}
		// Read from BoardPost_Server_fifo
		readBPostServer = read(bpost_server, msgFromBPost, MESSAGESIZE + 1);
		if (readBPostServer < 0) {
			perror("\nBoardServer: fifo read error\nDescription");
			exit(5);
		} else if (readBPostServer > 0) {
			// Get the offset of the received message
			int i = 0;
			int offsetSize;
			char *offset = malloc(BUFFERSIZE);
			while (*(msgFromBPost + i) != '/') {
				offset[i] = msgFromBPost[i];
				i++;
			}
			offset[i + 1] = '\0';
			offsetSize = i + 1;
			// Split the offset to words
			char* token;
			char** words = NULL;
			int numberOfSpaces = 0, tokencounter = 0;
			token = strtok(offset, " ");
			while (token != NULL) {
				numberOfSpaces++;
				words = realloc(words, sizeof(char*) * numberOfSpaces);
				if (words == NULL) {
					perror("\nBoardServer: malloc error\nDescription");
					exit(1);
				}
				words[tokencounter] = token;
				tokencounter++;
				token = strtok(NULL, " ");
			}
			int server_bpost;
			// Open Server_BoardPost_fifo
			if ((server_bpost = open(serverToBoardPost, O_WRONLY | O_NONBLOCK)) < 0) {
				perror("\nBoardServer : fifo open error\n");
				exit(3);
			}
			// List
			// words[0] == LST
			if (strcmp(words[0], "LST") == 0) {
				char feedbackList[MESSAGESIZE] = "active channels: \n\n";
				int iter1 = 0, noChannels = 1;
				char charId[10];
				while (iter1 < savedData.maxActiveChannels) {
					if (savedData.channelArray[iter1].id != -1) {
						snprintf(charId, 10, "%d", savedData.channelArray[iter1].id);
						strcat(feedbackList, "channel name: ");
						strncat(feedbackList, savedData.channelArray[iter1].name, 64);
						strcat(feedbackList, "\n");
						strcat(feedbackList, "channel id: ");
						strncat(feedbackList, charId, 10);
						strcat(feedbackList, "\n\n");
						noChannels = 0;
					}
					iter1++;
				}
				if (noChannels == 1) {
					strncpy(feedbackList, "NO_CH", MESSAGESIZE);
					int writeServerToBPost;
					if ((writeServerToBPost = write(server_bpost, feedbackList, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
					usleep(100000);
				} else {
					int writeServerToBPost;
					if ((writeServerToBPost = write(server_bpost, feedbackList, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
					usleep(100000);
				}
			}
			// Receive message
			// words[0] == MSG, words[1] == ch_id
			else if (strcmp(words[0], "MSG") == 0) {
				char feedbackMessage[MESSAGESIZE];
				int iter1 = 0, chFound = 0;
				int positionOfChannelInArray;
				int chId = atoi(words[1]);
				while (iter1 < savedData.maxActiveChannels && chFound == 0) {
					if (savedData.channelArray[iter1].id == chId) {
						positionOfChannelInArray = iter1;
						chFound = 1;
					}
					iter1++;
				}
				// If channel does not exist
				// Return error
				if (chFound == 0) {
					strncpy(feedbackMessage, "NO_CH", MESSAGESIZE);
					int writeServerToBPost;
					if ((writeServerToBPost = write(server_bpost, feedbackMessage, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
					usleep(100000);
				}
				// If channel exists save the message
				else {
					// Check if there is available space, if not then reallocate...
					if (savedData.channelArray[positionOfChannelInArray].maxMessages
							== savedData.channelArray[positionOfChannelInArray].numberOfMessages) {
						int oldSize = savedData.channelArray[positionOfChannelInArray].maxMessages;
						int doubleSize = savedData.channelArray[positionOfChannelInArray].maxMessages * 2;
						savedData.channelArray[positionOfChannelInArray].messages = realloc(
								savedData.channelArray[positionOfChannelInArray].messages, sizeof(char *) * doubleSize);
						// Reinitialize
						savedData.channelArray[positionOfChannelInArray].maxMessages = doubleSize;
						int j;
						for (j = 0; j < savedData.channelArray[positionOfChannelInArray].maxMessages; j++) {
							if (j >= oldSize) {
								savedData.channelArray[positionOfChannelInArray].messages[j] = malloc(
										sizeof(char) * MESSAGESIZE);
								if (savedData.channelArray[positionOfChannelInArray].messages[j] == NULL) {
									perror("\nBoardServer: malloc error\nDescription");
									exit(1);
								}
								strcpy(savedData.channelArray[positionOfChannelInArray].messages[j], "NOMSG");
							}
						}
					}
					// Save the message
					int iter2 = 0, stored = 0;
					while (iter2 < savedData.channelArray[positionOfChannelInArray].maxMessages && stored == 0) {
						if (strcmp(savedData.channelArray[positionOfChannelInArray].messages[iter2], "NOMSG") == 0) {
							// Remove the offset of the message and save it to the data struct
							memmove(savedData.channelArray[positionOfChannelInArray].messages[iter2],
									msgFromBPost + offsetSize, MESSAGESIZE);
							savedData.channelArray[positionOfChannelInArray].numberOfMessages++;
							stored = 1;
						}
						iter2++;
					}
					// Send feedback
					strncpy(feedbackMessage, "OK", MESSAGESIZE);
					int writeServerToBPost;
					if ((writeServerToBPost = write(server_bpost, feedbackMessage, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
					usleep(100000);
				}
			}
			// Receive file
			// words[0] == FLE, words[1] == ch_id, words[2] == file_name
			else if (strcmp(words[0], "FLE") == 0) {
				char feedbackMessage[MESSAGESIZE];
				int iter1 = 0, chFound = 0;
				int positionOfChannelInArray;
				int chId = atoi(words[1]);
				while (iter1 < savedData.maxActiveChannels && chFound == 0) {
					if (savedData.channelArray[iter1].id == chId) {
						positionOfChannelInArray = iter1;
						chFound = 1;
					}
					iter1++;
				}
				// If channel does not exist
				// Return error
				if (chFound == 0) {
					strncpy(feedbackMessage, "NO_CH", MESSAGESIZE);
					int writeServerToBPost;
					if ((writeServerToBPost = write(server_bpost, feedbackMessage, MESSAGESIZE)) == -1) {
						perror("\nBoardServer: fifo write error\nDescription");
						exit(2);
					}
					usleep(100000);
				}
				// If channel exists save the file
				else {
					// Check if there is available space, if not then reallocate...
					if (savedData.channelArray[positionOfChannelInArray].maxFiles
							== savedData.channelArray[positionOfChannelInArray].numberOfFiles) {
						int doubleSize = savedData.channelArray[positionOfChannelInArray].maxFiles * 2;
						savedData.channelArray[positionOfChannelInArray].filesFDs = realloc(
								savedData.channelArray[positionOfChannelInArray].filesFDs, sizeof(FILE*) * doubleSize);
						// Reinitialize
						savedData.channelArray[positionOfChannelInArray].maxFiles = doubleSize;
					}
					// Save the file
					int iter2 = 0, stored = 0;
					while (iter2 < savedData.channelArray[positionOfChannelInArray].maxFiles && stored == 0) {
						if (savedData.channelArray[positionOfChannelInArray].filesFDs[iter2] == NULL) {
							// Save the file descriptor
							char *pathOfTempFiles = malloc(BUFFERSIZE);
							if (pathOfTempFiles == NULL) {
								perror("\nBoardServer: malloc error\nDescription");
								exit(1);
							}
							// Save the temp file in the folder which contains the boards
							strcpy(pathOfTempFiles, path);
							dirname(pathOfTempFiles);
							strcat(pathOfTempFiles, "/");
							strcat(pathOfTempFiles, words[2]);
							FILE *writeFD = fopen(pathOfTempFiles, "w");
							savedData.channelArray[positionOfChannelInArray].filesFDs[iter2] = fopen(pathOfTempFiles,
									"r");
							savedData.channelArray[positionOfChannelInArray].numberOfFiles++;
							stored = 1;
							free(pathOfTempFiles);
							// Send "STARTSEND" to boardpost to start sending the file
							strncpy(feedbackMessage, "STARTSEND", MESSAGESIZE);
							int writeServerToBPost;
							if ((writeServerToBPost = write(server_bpost, feedbackMessage, MESSAGESIZE)) == -1) {
								perror("\nBoardServer: fifo write error\nDescription");
								exit(2);
							}
							usleep(100000);
							char endOfFile = 0;
							while (endOfFile == 0) {
								readBPostServer = read(bpost_server, msgFromBPost, MESSAGESIZE);
								if (strncmp(msgFromBPost, "END", 3) != 0) {
									fprintf(writeFD, "%s\n", msgFromBPost);
									// Write to the temp file immediately!!
									fflush(writeFD);
								} else {
									endOfFile = 1;
								}
							}
						}
						iter2++;
					}
				}
			}
			close(server_bpost);
			free(offset);
			free(words);
			free(token);
		}
		usleep(100000);
	}
}
