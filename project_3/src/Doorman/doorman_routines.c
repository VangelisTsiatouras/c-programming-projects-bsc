/*
 * doorman_routines.c
 *
 *  Created on: Dec 12, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "global_data_structures.h"

void Doorman(int period, int shmid) {
	/* Check if log file exists in restaurant folder
	 * If not create a log file in the folder of customer
	 */
	FILE *logfile;
	if (access("../Restaurant/log.txt", F_OK) != -1) {
		logfile = fopen("../Restaurant/log.txt", "a+");
	} else {
		logfile = fopen("log.txt", "w+");
	}
	pid_t pid = getpid();
	fprintf(logfile, "[PID: %d] (DOORMAN): Doorman created\n", pid);
	fflush(logfile);
	sem_t *semDoorQueue = sem_open(SEMDOORQUEUE, 0);
	if (semDoorQueue == SEM_FAILED) {
		printf("semaphore open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semDoormanWait1 = sem_open(SEMDOORMANWAIT1, 0);
	if (semDoormanWait1 == SEM_FAILED) {
		printf("semaphore open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semDoormanWait2 = sem_open(SEMDOORMANWAIT2, 0);
	if (semDoormanWait2 == SEM_FAILED) {
		printf("semaphore open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semDoormanWait3 = sem_open(SEMDOORMANWAIT3, 0);
	if (semDoormanWait3 == SEM_FAILED) {
		printf("semaphore open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semCustomerWait = sem_open(SEMCUSTOMERWAIT, O_CREAT, 0660, 0);
	if (semCustomerWait == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semBar = sem_open(SEMBAR, O_CREAT, 0660, 0);
	if (semBar == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	RestaurantStruct *restaurant;
	/* Attach the shared memory segment */
	restaurant = shmat(shmid, NULL, 0);
	if (restaurant == NULL) {
		printf("shared memory attach error %s\n", strerror(errno));
		shmctl(shmid, IPC_RMID, NULL);
		exit(EXIT_FAILURE);
	}
	int maxSizeTable = 0;
	int i, j;
	for (i = 0; i < restaurant->numberOfTables; i++) {
		if (restaurant->tables[i].size > maxSizeTable) {
			maxSizeTable = restaurant->tables[i].size;
		}
	}
	int servicedCustomers = 0, barIndex = 0, customerBarServiced, tableFound,
			tableNumber, breakLoop;
	int tempGroupSize, tempGroupPID, doorTime;
	while (1) {
		if (restaurant->stats.totalCustomersArrived
				== restaurant->stats.targetNumberOfCustomers) {
			break;
		}
		/* Check the bar*/
		customerBarServiced = 0;
		breakLoop = 0;
		while (restaurant->bar.occupiedSeats > 0 && breakLoop == 0) {
			for (i = 0; i < restaurant->bar.capacity; i++) {
				tableFound = 0;
				if (restaurant->bar.groupsWaitingAtBar[i].groupPID != 0) {
					/* Find a table*/
					for (j = 0; j < restaurant->numberOfTables; j++) {
						if (restaurant->tables[j].occupied == 0
								&& restaurant->tables[j].size
										>= restaurant->bar.groupsWaitingAtBar[i].groupSize) {
							fprintf(logfile,
									"[PID: %d] (DOORMAN): Servicing customer (PID: %d) at the bar\n",
									pid,
									restaurant->bar.groupsWaitingAtBar[i].groupPID);
							fflush(logfile);
							tableNumber = j;
							tableFound = 1;
							fprintf(logfile,
									"[PID: %d] (DOORMAN): Send the table number (tableID %d) to customer (PID: %d) who waits at bar at %d position\n",
									pid, tableNumber,
									restaurant->bar.groupsWaitingAtBar[i].groupPID,
									i);
							fflush(logfile);
							restaurant->bar.groupsWaitingAtBar[i].gotoTableID =
									tableNumber;
							customerBarServiced = 1;
							sem_wait(semDoormanWait3);
							break;
						}
					}
				}
			}
			if (tableFound == 0) {
				breakLoop = 1;
			}
		}
		/* Check the door*/
		if (customerBarServiced == 0
				&& servicedCustomers
						< restaurant->stats.targetNumberOfCustomers) {
			fprintf(logfile,
					"[PID: %d] (DOORMAN): Servicing customer at the door\n",
					pid);
			fflush(logfile);
			sem_post(semDoorQueue);
			/* Wait until customer defines the group size*/
			fprintf(logfile,
					"[PID: %d] (DOORMAN): Wait for customer to define the group size\n",
					pid);
			fflush(logfile);
			/* Wait for customer to define sizes etc*/
			sem_wait(semDoormanWait1);
			tempGroupSize = restaurant->door.customerGroupSize;
			tempGroupPID = restaurant->door.groupPID;
			/* Check if the group can fit on table*/
			if (tempGroupSize > maxSizeTable) {
				fprintf(logfile,
						"[PID: %d] (DOORMAN): Group too big. No tables with size %d\n",
						pid, tempGroupSize);
				fflush(logfile);
				doorTime = rand() % (period) + 1;
				sleep(doorTime);
				restaurant->door.gotoBar = -1;
				restaurant->door.gotoTableID = -1;
				restaurant->door.leave = 1;
				servicedCustomers++;
				sem_post(semCustomerWait);
				sem_wait(semDoormanWait2);
			} else {
				fprintf(logfile,
						"[PID: %d] (DOORMAN): Trying to find a table with size %d\n",
						pid, tempGroupSize);
				fflush(logfile);
				doorTime = rand() % (period) + 1;
				sleep(doorTime);
				/* Find a table*/
				tableFound = 0;
				for (i = 0; i < restaurant->numberOfTables; i++) {
					if (restaurant->tables[i].occupied == 0
							&& restaurant->tables[i].size >= tempGroupSize) {
						tableNumber = i;
						tableFound = 1;
						break;
					}
				}
				/* If table found*/
				if (tableFound != 0) {
					fprintf(logfile,
							"[PID: %d] (DOORMAN): Send the table number to customer (PID: %d)\n",
							pid, tempGroupPID);
					fflush(logfile);
					restaurant->door.gotoTableID = tableNumber;
					servicedCustomers++;
				}
				/* Check the bar for empty space*/
				else {
					/* If there is space at bar to fit the group*/
					if (tempGroupSize
							<= (restaurant->bar.capacity
									- restaurant->bar.occupiedSeats)) {
						int position;
						/* Find empty space at the bar*/
						i = 0;
						while (restaurant->bar.groupsWaitingAtBar[i].groupPID
								!= 0) {
							i++;
						}
						fprintf(logfile,
								"[PID: %d] (DOORMAN): Send customer (PID: %d) to the bar\n",
								pid, tempGroupPID);
						fflush(logfile);
						restaurant->door.gotoBar = i;
						restaurant->door.gotoTableID = -1;
						restaurant->door.leave = 0;
						servicedCustomers++;
					}
					/* If not tell the group to leave*/
					else {
						fprintf(logfile,
								"[PID: %d] (DOORMAN): Tell to customer (PID: %d) to leave because the restaurant is full\n",
								pid, tempGroupPID);
						fflush(logfile);
						restaurant->door.gotoBar = -1;
						restaurant->door.gotoTableID = -1;
						restaurant->door.leave = 1;
						servicedCustomers++;
					}
				}
				sem_post(semCustomerWait);
				/* Wait for customer to sit at bar/table*/
				sem_wait(semDoormanWait2);
			}
		}
	}
	fflush(stdout);
	/* Detach*/
	if (shmdt((void *) restaurant) == -1) {
		printf("detach error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_close(semDoorQueue);
	sem_close(semDoormanWait1);
	sem_close(semDoormanWait2);
	sem_close(semDoormanWait3);
	sem_close(semCustomerWait);
	sem_close(semBar);
	fclose(logfile);
}
