/*
 * master_thread.c
 *
 *  Created on: May 23, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "hashtable.h"
#include "threadpool.h"
#include "worker_thread.h"
#include "declare_globals.h"

void MasterThread(int port, int queue_size) {
	int sock, newsock;
	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *clientptr = (struct sockaddr *) &client;
	/* Create socket */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error %s\n", strerror(errno));
	}
	/* Internet domain */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl( INADDR_ANY);
	/* The given port */
	server.sin_port = htons(port);
	/* Bind socket to address */
	if (bind(sock, serverptr, sizeof(server)) < 0) {
		printf("bind error %s\n", strerror(errno));
	}
	/* Listen for connections */
	if (listen(sock, 5) < 0) {
		printf("listen error %s\n", strerror(errno));
	}
	printf("Listening for connections to port %d\n", port);
	while (1) {
		/* Accept TCP connection */
		if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
			printf("accept error %s\n", strerror(errno));
		}
		/* Find client ’s address */
		printf(" Accepted connection \n");
		/* Place socket file descriptor to thread pool */
		pthread_mutex_lock(&mtx);
		while (threadPool.count >= threadPool.size) {
			printf(">> Found Buffer Full\n");
			pthread_cond_wait(&cond_nonfull, &mtx);
		}
		threadPool.end = (threadPool.end + 1) % threadPool.size;
		threadPool.socketFileDescriptor[threadPool.end] = newsock;
		threadPool.count++;
		pthread_cond_signal(&cond_nonempty);
		pthread_mutex_unlock(&mtx);
	}
}
