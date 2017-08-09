/*
 * customer_routines.c
 *
 *  Created on: Dec 10, 2016
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

void Customer(int people, int period, int shmid) {
	/* Check if log file exists in restaurant folder
	 * If not create a log file in the folder of customer
	 */
	srand(time(NULL) ^ (getpid() << 16));
	int spawnCustomerTime = rand() % 10;
	FILE *logfile;
	if (access("../Restaurant/log.txt", F_OK) != -1) {
		logfile = fopen("../Restaurant/log.txt", "a+");
	} else {
		logfile = fopen("log.txt", "w+");
	}
	pid_t pid = getpid();
	fprintf(logfile,
			"[PID: %d] (CUSTOMER): Customer created, %d seconds to go to restaurant\n",
			pid, spawnCustomerTime);
	sleep(spawnCustomerTime);
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
	sem_t *semStats = sem_open(SEMSTATS, O_CREAT, 0660, 0);
	if (semStats == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semBar = sem_open(SEMBAR, O_CREAT, 0660, 0);
	if (semBar == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fprintf(logfile, "[PID: %d] (CUSTOMER): Just arrived at door and waiting\n",
			pid);
	fflush(logfile);
	sem_wait(semDoorQueue);
	fprintf(logfile,
			"[PID: %d] (CUSTOMER): Communicating with doorman. Send the size of the group which is %d\n",
			pid, people);
	fflush(logfile);
	RestaurantStruct *restaurant;
	/* Attach the shared memory segment */
	restaurant = shmat(shmid, NULL, 0);
	if (restaurant == NULL) {
		printf("shared memory attach error %s\n", strerror(errno));
		shmctl(shmid, IPC_RMID, NULL);
		exit(EXIT_FAILURE);
	}
	restaurant->door.customerGroupSize = people;
	restaurant->door.groupPID = pid;
	sem_post(semDoormanWait1);
	/* Wait for doorman to find a table*/
	fprintf(logfile,
			"[PID: %d] (CUSTOMER): Waiting for doorman to find a table\n", pid);
	fflush(logfile);
	sem_wait(semCustomerWait);
	int gotoTable = 0, tableID, eatingTime;
	if (restaurant->door.gotoBar > -1) {
		/* Possibility if the customer does not
		 * choose to go to the bar.
		 * Random number 0-100, if it is less than  equal to 25
		 * then choose to leave.
		 */
		int noWaitBar = rand() % 100;
		if (noWaitBar <= 25) {
			fprintf(logfile,
					"[PID: %d] (CUSTOMER): I choose to not wait at the bar. I am leaving.\n",
					pid);
			fflush(logfile);
			restaurant->door.gotoTableID = -1;
			restaurant->door.gotoBar = -1;
			restaurant->door.customerGroupSize = 0;
			restaurant->door.groupPID = 0;
			restaurant->door.leave = 0;
			gotoTable = -1;
			sem_wait(semStats);
			restaurant->stats.totalCustomersArrived++;
			restaurant->stats.customersLeft++;
			sem_post(semStats);
			sem_post(semDoormanWait2);
		} else {
			fprintf(logfile, "[PID: %d] (CUSTOMER): Waiting at the bar\n", pid);
			fflush(logfile);
			int barPosition = restaurant->door.gotoBar;
			restaurant->door.gotoTableID = -1;
			restaurant->door.gotoBar = -1;
			restaurant->door.customerGroupSize = 0;
			restaurant->door.groupPID = 0;
			restaurant->door.leave = 0;
			/* Lock the bar*/
			sem_wait(semBar);
			restaurant->bar.groupsWaitingAtBar[barPosition].groupPID = pid;
			restaurant->bar.groupsWaitingAtBar[barPosition].gotoTableID = -1;
			restaurant->bar.groupsWaitingAtBar[barPosition].positionBar =
					barPosition;
			restaurant->bar.groupsWaitingAtBar[barPosition].groupSize = people;
			restaurant->bar.occupiedSeats += people;
			sem_post(semBar);
			sem_post(semDoormanWait2);
			/* Possibility if the customer leaves the bar.
			 * Random number 0-100, if it is less than equal to 10
			 * then choose to leave.
			 */
			int noWaitBar, leaveBar = 0;
			noWaitBar = rand() % 100;
			while (restaurant->bar.groupsWaitingAtBar[barPosition].gotoTableID
					== -1) {
				usleep(10000);
				if (noWaitBar <= 10) {
					leaveBar = 1;
					break;
				}
			}
			if (leaveBar == 0) {
				/* Lock the bar*/
				fprintf(logfile,
						"[PID: %d] (CUSTOMER): Leaving from bar and going to table %d\n",
						pid,
						restaurant->bar.groupsWaitingAtBar[barPosition].gotoTableID);
				fflush(logfile);
				sem_wait(semBar);
				tableID =
						restaurant->bar.groupsWaitingAtBar[barPosition].gotoTableID;
				restaurant->bar.groupsWaitingAtBar[barPosition].groupPID = 0;
				restaurant->bar.groupsWaitingAtBar[barPosition].gotoTableID =
						-1;
				restaurant->bar.groupsWaitingAtBar[barPosition].positionBar =
						-1;
				restaurant->bar.groupsWaitingAtBar[barPosition].groupSize = 0;
				restaurant->bar.occupiedSeats -= people;
				gotoTable = 1;
				restaurant->tables[tableID].occupied = 1;
				sem_post(semBar);
				sem_post(semDoormanWait3);
			} else if (leaveBar == 1) {
				fprintf(logfile,
						"[PID: %d] (CUSTOMER): Leaving bar. I waited too long!\n",
						pid);
				fflush(logfile);
				sem_wait(semBar);
				restaurant->bar.groupsWaitingAtBar[barPosition].groupPID = 0;
				restaurant->bar.groupsWaitingAtBar[barPosition].gotoTableID =
						-1;
				restaurant->bar.groupsWaitingAtBar[barPosition].positionBar =
						-1;
				restaurant->bar.groupsWaitingAtBar[barPosition].groupSize = 0;
				restaurant->bar.occupiedSeats -= people;
				gotoTable = -1;
				sem_wait(semStats);
				restaurant->stats.totalCustomersArrived++;
				restaurant->stats.customersLeft++;
				sem_post(semStats);
				sem_post(semBar);
			}
		}
	} else if (restaurant->door.gotoTableID > -1) {
		tableID = restaurant->door.gotoTableID;
		restaurant->door.gotoTableID = -1;
		restaurant->door.gotoBar = -1;
		restaurant->door.customerGroupSize = 0;
		restaurant->door.groupPID = 0;
		restaurant->door.leave = 0;
		restaurant->tables[tableID].occupied = 1;
		gotoTable = 1;
		sem_post(semDoormanWait2);
	} else if (restaurant->door.leave == 1) {
		restaurant->door.gotoTableID = -1;
		restaurant->door.gotoBar = -1;
		restaurant->door.customerGroupSize = 0;
		restaurant->door.groupPID = 0;
		restaurant->door.leave = 0;
		sem_post(semDoormanWait2);
		gotoTable = 0;
	}
	if (gotoTable == 1) {
		/* SIT ON TABLE*/
		sem_wait(semStats);
		restaurant->stats.tablesFull++;
		restaurant->stats.customersServiced++;
		sem_post(semStats);
		fprintf(logfile,
				"[PID: %d] (CUSTOMER): Just sit on table with number %d\n", pid,
				tableID);
		fflush(logfile);
		/* ORDER*/
		restaurant->tables[tableID].groupPID = pid;
		restaurant->tables[tableID].readyToOrder = 1;
		/* Wait waiter to order */
		fprintf(logfile,
				"[PID: %d] (CUSTOMER): Waiting for waiter to order at table %d\n",
				pid, tableID);
		fflush(logfile);
		sem_wait(&(restaurant->tables[tableID].semCustomerWait));
		sem_post(&(restaurant->tables[tableID].semWaiterWait));
		/* EAT*/
		eatingTime = rand() % (period) + 1;
		sleep(eatingTime);
		/* Wait waiter to pay*/
		fprintf(logfile,
				"[PID: %d] (CUSTOMER): Waiting for waiter to pay at table %d\n",
				pid, tableID);
		fflush(logfile);
		restaurant->tables[tableID].readyToPay = 1;
		sem_wait(&(restaurant->tables[tableID].semCustomerWait));
		/* Leave table and restaurant*/
		sem_wait(semStats);
		restaurant->stats.totalCustomersArrived++;
		restaurant->stats.tablesFull--;
		sem_post(semStats);
		fprintf(logfile, "[PID: %d] (CUSTOMER): Leaving restaurant\n", pid);
		fflush(logfile);
		restaurant->tables[tableID].groupPID = 0;
		restaurant->tables[tableID].occupied = 0;
	} else if (gotoTable == 0) {
		sem_wait(semStats);
		restaurant->stats.customersLeft++;
		restaurant->stats.totalCustomersArrived++;
		sem_post(semStats);
		fprintf(logfile,
				"[PID: %d] (CUSTOMER): Leaving restaurant because no space\n",
				pid);
		fflush(logfile);
	}
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
	sem_close(semStats);
	sem_close(semBar);
	fclose(logfile);
}
