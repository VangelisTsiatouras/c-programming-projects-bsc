/*
 * main.c
 *
 *  Created on: May 25, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "client.h"

int main(int argc, char **argv) {
	int i, minusH = 0, minusP = 0, minusI = 0, NOserver_host = 0, NOserver_port = 0, NOcommand_file = 0;
	int server_port;
	char server_host[HOST_NAME_MAX], command_file[192];
	/* Split args */
	if (argc > 7) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 7) {
		printf("too few arguments!\n");
		return 1;
	} else {
		for (i = 0; i < argc; ++i) {
			if (strncmp(argv[i], "-h", 2) == 0) {
				minusH = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-p", 2) != 0 && strncmp(argv[i + 1], "-i", 2) != 0) {
					strncpy(server_host, argv[i + 1], HOST_NAME_MAX);
				} else {
					NOserver_host = 1;
				}
			} else if (strcmp(argv[i], "-p") == 0) {
				minusP = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-h", 2) != 0 && strncmp(argv[i + 1], "-i", 2) != 0) {
					server_port = atoi(argv[i + 1]);
				} else {
					NOserver_port = 1;
				}
			} else if (strcmp(argv[i], "-i") == 0) {
				minusI = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-h", 2) != 0 && strncmp(argv[i + 1], "-p", 2) != 0) {
					strncpy(command_file, argv[i + 1], 192);
				} else {
					NOcommand_file = 1;
				}
			}
		}
	}
	if (minusH == 0) {
		printf("flag -h is missing\ntype -h <server_host>\nclient exiting\n");
		return 1;
	}
	if (minusP == 0) {
		printf("flag -p is missing\ntype -o <server_port>\nclient exiting\n");
		return 1;
	}
	if (minusI == 0) {
		printf("flag -i is missing\ntype -i <command_file>\nclient exiting\n");
		return 1;
	}
	if (NOserver_host == 1) {
		printf("server host not entered\ntype -h <server_host>\nclient exiting\n");
		return 1;
	}
	if (NOserver_port == 1) {
		printf("number of server port not entered\ntype -o <server_port>\nclient exiting\n");
		return 1;
	}
	if (NOcommand_file == 1) {
		printf("command file not entered\ntype -i <command_file>\nclient exiting\n");
		return 1;
	}
	if (server_port <= 0) {
		printf("the server port must be greater than 0\nclient exiting\n");
		return 1;
	}
	printf("port: %d\n", server_port);
	Client(server_host, server_port, command_file);
}
