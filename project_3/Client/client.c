/*
 * client.c
 *
 *  Created on: May 25, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

#define BUFFERSIZE 2048

void Client(char *servHost, int servPort, char *commandFile) {
	char buf[BUFFERSIZE];
	FILE* cmdFile;
	int sock;
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct hostent *rem;
	/* Create socket */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		exit(1);
	}
	/* Find server address */
	if ((rem = gethostbyname(servHost)) == NULL) {
		perror("gethostbyname error");
		exit(1);
	}
	/* Internet domain */
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	/* Server port */
	server.sin_port = htons(servPort);
	/* Initiate TCP connection */
	if (connect(sock, serverptr, sizeof(server)) < 0) {
		perror("connect error");
		exit(1);
	}
	printf("**********  CLIENT  **********\n");
	printf("Connecting to %s port %d\n", servHost, servPort);
	/* Open the command file */
	printf("Commandfile %s\n", commandFile);
	cmdFile = fopen(commandFile, "r");
	if (cmdFile == NULL) {
		printf("FAILURE: file not found!\n		Client exiting\n");
		exit(1);
	}
	/* Read the command file line by line */
	char feedback[BUFFERSIZE];
	int sleep;
	while ((fgets(buf, BUFFERSIZE, cmdFile) != NULL)) {
		printf("%s\n", buf);
		/* Ignore new line */
		if (strncmp(buf, "\n", 1) == 0) {
			continue;
		}
		/* Exit */
		else if (strncmp(buf, "exit", 4) == 0) {
			/* Send message to server to close the socket */
			if (write(sock, "CLOSE", 5) < 0) {
				perror("write error");
				exit(1);
			}
			/* Close socket and exit */
			close(sock);
			return;
		}
		/* Sleep */
		else if (strncmp(buf, "sleep", 5) == 0) {
			/* Do some sleep */
			sscanf(buf, "sleep %d", &sleep);
			usleep(1000 * sleep);
			printf("--- Sleep finished\n");
		} else if ((strncmp(buf, "add_account", 11) == 0) || (strncmp(buf, "add_transfer", 12) == 0)
				|| (strncmp(buf, "add_multi_transfer", 18) == 0) || (strncmp(buf, "print_balance", 13) == 0)
				|| (strncmp(buf, "print_multi_balance", 19) == 0)) {
			if (write(sock, buf, BUFFERSIZE) < 0) {
				perror("write error");
				exit(1);
			}
			/* Receive results from server */
			while ((read(sock, feedback, BUFFERSIZE) > 0) && (strncmp(feedback, "END_OF_PRINT", BUFFERSIZE) != 0)) {
				printf("--- %s\n", feedback);
			}
		} else {
			printf("--- FAILURE: Unknown command\n");
			continue;
		}
	}
	/* Read from stdin */
	while (fgets(buf, BUFFERSIZE, stdin) != NULL) {
		/* Ignore new line */
		if (strncmp(buf, "\n", 1) == 0) {
			continue;
		}
		/* Exit */
		else if (strncmp(buf, "exit", 4) == 0) {
			/* Send message to server to close the socket */
			if (write(sock, "CLOSE", 5) < 0) {
				perror("write error");
				exit(1);
			}
			/* Close socket and exit */
			close(sock);
			return;
		}
		/* Sleep */
		else if (strncmp(buf, "sleep", 5) == 0) {
			/* Do some sleep */
			sscanf(buf, "sleep %d", &sleep);
			usleep(1000 * sleep);
			printf("--- Sleep finished %d\n", sleep);
		} else if ((strncmp(buf, "add_account", 11) == 0) || (strncmp(buf, "add_transfer", 12) == 0)
				|| (strncmp(buf, "add_multi_transfer", 18) == 0) || (strncmp(buf, "print_balance", 13) == 0)
				|| (strncmp(buf, "print_multi_balance", 19) == 0)) {
			if (write(sock, buf, BUFFERSIZE) < 0) {
				perror("write error");
				exit(1);
			}
			/* Receive results from server */
			while ((read(sock, feedback, BUFFERSIZE) > 0) && (strncmp(feedback, "END_OF_PRINT", BUFFERSIZE) != 0)) {
				printf("--- %s\n", feedback);
			}
		} else {
			printf("--- FAILURE: Unknown command\n");
			continue;
		}
	}
}
