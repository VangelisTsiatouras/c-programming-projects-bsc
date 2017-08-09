/*
 * restaurant_routines.c
 *
 *  Created on: Dec 8, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "global_data_structures.h"
#include "shared_memory_functions.h"

#define BUFFERSIZE 256

void RestaurantRoutine(int customers, int period, int numberOfWaiters, int numberOfTables, int maxSizeOfTable,
		int customerPeriod, int doormanPeriod, int waiterPeriod, int moneyamount) {
	int status, i;
	pid_t waitPID;
	FILE *logfile;
	logfile = fopen("log.txt", "w+");
	pid_t doormanPID;
	/* Create array to store PIDs of the waiters*/
	pid_t *waitersPIDs = (pid_t *) malloc(sizeof(pid_t) * (numberOfWaiters));
	if (waitersPIDs == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Create array to store PIDs of the customers*/
	pid_t *customersPIDs = (pid_t *) malloc(sizeof(pid_t) * (customers));
	if (customersPIDs == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Initialize shared memory segment
	 * Create the Restaurant*/
	int shmid = SharedMemInit(numberOfTables, maxSizeOfTable, numberOfWaiters, customers);
	sem_t *semDoorQueue = sem_open(SEMDOORQUEUE, 0);
	/* Attach*/
	RestaurantStruct *restaurant;
	restaurant = shmat(shmid, NULL, 0);
	if (restaurant == NULL) {
		printf("shared memory attach error %s\n", strerror(errno));
		shmctl(shmid, IPC_RMID, NULL);
		exit(EXIT_FAILURE);
	}
	/* Create Waiters*/
	for (i = 0; i < numberOfWaiters; i++) {
		waitersPIDs[i] = fork();
		if (waitersPIDs[i] < 0) {
			printf("fork error %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* Waiter*/
		else if (waitersPIDs[i] == 0) {
			char chPeriod_W[BUFFERSIZE];
			char chMoneyamount[BUFFERSIZE];
			char chShmid[BUFFERSIZE];
			snprintf(chPeriod_W, BUFFERSIZE, "%d", waiterPeriod);
			snprintf(chMoneyamount, BUFFERSIZE, "%d", moneyamount);
			snprintf(chShmid, BUFFERSIZE, "%d", shmid);
			if (execlp("../Waiter/waiter", "waiter", "-d", chPeriod_W, "-m", chMoneyamount, "-s", chShmid, NULL)
					< 0) {
				printf("execl error %s\n", strerror(errno));
				exit(1);
			}
		}
	}
	/* Create Doorman*/
	doormanPID = fork();
	if (doormanPID == 0) {
		char chPeriod_D[BUFFERSIZE];
		char chShmid[BUFFERSIZE];
		snprintf(chPeriod_D, BUFFERSIZE, "%d", doormanPeriod);
		snprintf(chShmid, BUFFERSIZE, "%d", shmid);
		if (execlp("../Doorman/doorman", "doorman", "-d", chPeriod_D, "-s", chShmid, NULL) < 0) {
			printf("execl error %s\n", strerror(errno));
			exit(1);
		}
	}
	/* Create Customer Groups*/
	for (i = 0; i < customers; i++) {
		customersPIDs[i] = fork();
		if (customersPIDs[i] < 0) {
			printf("fork error %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* Customer Group*/
		else if (customersPIDs[i] == 0) {
			srand(time(NULL) ^ (getpid() << 16));
			int people = rand() % (8) + 1;
			char chPeriod_C[BUFFERSIZE];
			char chPeople[BUFFERSIZE];
			char chShmid[BUFFERSIZE];
			snprintf(chPeriod_C, BUFFERSIZE, "%d", customerPeriod);
			snprintf(chPeople, BUFFERSIZE, "%d", people);
			snprintf(chShmid, BUFFERSIZE, "%d", shmid);
			if (execlp("../Customer/customer", "customer", "-n", chPeople, "-d", chPeriod_C, "-s", chShmid,
			NULL) < 0) {
				printf("execl error %s\n", strerror(errno));
				exit(1);
			}
		}
	}
	int j;
	while (restaurant->stats.totalCustomersArrived < restaurant->stats.targetNumberOfCustomers) {
		sleep(period);
		printf("Income: %d$\nGroups left: %d\nCustomers serviced: %d\n", restaurant->stats.totalAmount,
				restaurant->stats.customersLeft, restaurant->stats.customersServiced);
		for (i = 0; i < restaurant->numberOfTables; i++) {
			printf("Table ID %d, Size %d ", i, restaurant->tables[i].size);
			fflush(stdout);
			if (restaurant->tables[i].occupied == 1) {
				printf("Occupied: YES\n");
			} else {
				printf("Occupied: NO\n");
			}
			fflush(stdout);
		}
		printf("BAR: occupied seats %d\n", restaurant->bar.occupiedSeats);
		for (j = 0; j < 15; j++) {
			if (restaurant->bar.groupsWaitingAtBar[j].groupPID != 0) {
				printf("Bar seat %d, groupPID %d, Group size %d\n", restaurant->bar.groupsWaitingAtBar[j].positionBar,
						restaurant->bar.groupsWaitingAtBar[j].groupPID,
						restaurant->bar.groupsWaitingAtBar[j].groupSize);
				fflush(stdout);
			}
		}
	}
	printf("Income: %d$\nGroups left: %d\nCustomers serviced: %d\n", restaurant->stats.totalAmount,
			restaurant->stats.customersLeft, restaurant->stats.customersServiced);
	/* WAIT FOR ALL THE CHILD PROCESSES TO TERMINATE*/
	while ((waitPID = wait(&status)) > 0) {
	}
	/* Purge shared memory segment
	 * Destroy the Restaurant*/
	SharedMemPurge(shmid);
	fclose(logfile);
	free(waitersPIDs);
	free(customersPIDs);
}
