/*
 * waiter_routines.c
 *
 *  Created on: Dec 24, 2016
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

void Waiter(int period, int moneyAmount, int shmid) {
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
	fprintf(logfile, "[PID: %d] (WAITER): Waiter created\n", pid);
	fflush(logfile);
	sem_t *semStats = sem_open(SEMSTATS, O_CREAT, 0660, 0);
	if (semStats == SEM_FAILED) {
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
	int i, value, orderingTime, payingTime, payment;
	/* Create an array which contains the tables that the waiter is servicing*/
	int *tablesServicing = (int *) malloc(sizeof(int) * restaurant->numberOfTables);
	if (tablesServicing == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < restaurant->numberOfTables; i++) {
		tablesServicing[i] = 0;
	}
	while (1) {
		if (restaurant->stats.totalCustomersArrived == restaurant->stats.targetNumberOfCustomers) {
			break;
		}
		/* Check for new customers at tables*/
		for (i = 0; i < restaurant->numberOfTables; i++) {
			sem_getvalue(&(restaurant->tables[i].semWaiterWait), &value);
			if (restaurant->tables[i].occupied == 1 && restaurant->tables[i].readyToOrder == 1 && value == 1) {
				if (sem_trywait(&(restaurant->tables[i].semWaiterWait)) == 0) {
					fprintf(logfile, "[PID: %d] (WAITER): Servicing customer (PID: %d) at table %d\n", pid,
							restaurant->tables[i].groupPID, i);
					fflush(logfile);
					orderingTime = rand() % (period) + 1;
					sleep(orderingTime);
					restaurant->tables[i].readyToOrder = 0;
					tablesServicing[i] = 1;
					sem_post(&(restaurant->tables[i].semCustomerWait));
				}
			}
		}
		/* Check if a customer wants to pay*/
		for (i = 0; i < restaurant->numberOfTables; i++) {
			if (tablesServicing[i] == 1 && restaurant->tables[i].readyToPay == 1) {

				fprintf(logfile, "[PID: %d] (WAITER): Customer (PID: %d) at table %d paying\n", pid,
						restaurant->tables[i].groupPID, i);
				fflush(logfile);
				payingTime = rand() % (period) + 1;
				sleep(payingTime);
				sem_wait(semStats);
				payment = rand() % (moneyAmount) + 1;
				restaurant->stats.totalAmount += payment;
				sem_post(semStats);
				restaurant->tables[i].readyToPay = 0;
				tablesServicing[i] = 0;
				sem_post(&(restaurant->tables[i].semCustomerWait));
			}
		}
	}
	free(tablesServicing);
	/* Detach*/
	if (shmdt((void *) restaurant) == -1) {
		printf("detach error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_close(semStats);
}

