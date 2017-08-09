/*
 * shared_memory_functions.c
 *
 *  Created on: Dec 3, 2016
 *      Author: vangelis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "global_data_structures.h"

int SharedMemInit(int numberOfTables, int maxSizeOfTables, int numberOfWaiters, int numberOfCustomers) {
	int shm_id, i, index;
	int tableSizes[] = { 2, 4, 6, 8 };
	/* Initialize semaphores*/
	sem_t *semDoorQueue = sem_open(SEMDOORQUEUE, O_CREAT, 0666, 0);
	if (semDoorQueue == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semDoormanWait1 = sem_open(SEMDOORMANWAIT1, O_CREAT, 0666, 0);
	if (semDoormanWait1 == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semDoormanWait2 = sem_open(SEMDOORMANWAIT2, O_CREAT, 0666, 0);
	if (semDoormanWait2 == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semDoormanWait3 = sem_open(SEMDOORMANWAIT3, O_CREAT, 0666, 0);
	if (semDoormanWait3 == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semCustomerWait = sem_open(SEMCUSTOMERWAIT, O_CREAT, 0666, 0);
	if (semCustomerWait == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semBar = sem_open(SEMBAR, O_CREAT, 0666, 1);
	if (semBar == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sem_t *semStats = sem_open(SEMSTATS, O_CREAT, 0666, 1);
	if (semStats == SEM_FAILED) {
		printf("semaphore initialize/open error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Create Restaurant*/
	RestaurantStruct *restaurant;
	/* Allocate an array which will have maximum size
	 * number of seats of the bar.
	 */
	int size = sizeof(Door) + sizeof(Bar) + sizeof(Stats) + sizeof(Table) * numberOfTables + sizeof(int) * 2;
	shm_id = shmget(IPC_PRIVATE, size, 0666);
	if (shm_id == -1) {
		printf("shared memory creation error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Attach the shared memory segment */
	restaurant = shmat(shm_id, NULL, 0);
	if (restaurant == NULL) {
		printf("shared memory attach error %s\n", strerror(errno));
		shmctl(shm_id, IPC_RMID, NULL);
		exit(EXIT_FAILURE);
	}
	/* Initialize Restaurant*/
	restaurant->stats.targetNumberOfCustomers = numberOfCustomers;
	restaurant->stats.totalAmount = 0;
	restaurant->stats.tablesFull = 0;
	restaurant->stats.totalCustomersArrived = 0;
	restaurant->stats.customersLeft = 0;
	restaurant->stats.customersServiced = 0;
	restaurant->door.customerGroupSize = 0;
	restaurant->door.groupPID = 0;
	restaurant->door.gotoBar = -1;
	restaurant->door.gotoTableID = -1;
	restaurant->door.leave = -1;
	restaurant->bar.capacity = 15;
	restaurant->bar.occupiedSeats = 0;
	for (i = 0; i < 15; i++) {
		restaurant->bar.groupsWaitingAtBar[i].positionBar = -1;
		restaurant->bar.groupsWaitingAtBar[i].groupPID = 0;
		restaurant->bar.groupsWaitingAtBar[i].groupSize = 0;
		restaurant->bar.groupsWaitingAtBar[i].gotoTableID = -1;
	}
	restaurant->numberOfWaiters = numberOfWaiters;
	restaurant->numberOfTables = numberOfTables;
	for (i = 0; i < numberOfTables; i++) {
		restaurant->tables[i].occupied = 0;
		/* Create random tables with capacity 2,4,6,8
		 * Choose the size from the tableSizes array
		 * with the help of the index
		 */
		index = rand() % 4;
		if ((sem_init(&(restaurant->tables[i].semCustomerWait), 1, 0)) != 0) {
			printf("semaphore initialize error %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((sem_init(&(restaurant->tables[i].semWaiterWait), 1, 1)) != 0) {
			printf("semaphore initialize error %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		restaurant->tables[i].groupPID = 0;
		restaurant->tables[i].size = tableSizes[index];
		restaurant->tables[i].occupied = 0;
		restaurant->tables[i].readyToOrder = 0;
		restaurant->tables[i].readyToPay = 0;

	}
	return shm_id;
}

int SharedMemPurge(int shmid) {
	int i;
	RestaurantStruct *restaurant;
	restaurant = shmat(shmid, NULL, 0);
	if (restaurant == NULL) {
		printf("shared memory attach error %s\n", strerror(errno));
		shmctl(shmid, IPC_RMID, NULL);
		exit(EXIT_FAILURE);
	}
	sem_t *semDoorQueue = sem_open(SEMDOORQUEUE, 0);
	sem_t *semDoormanWait1 = sem_open(SEMDOORMANWAIT1, 0);
	sem_t *semDoormanWait2 = sem_open(SEMDOORMANWAIT2, 0);
	sem_t *semDoormanWait3 = sem_open(SEMDOORMANWAIT3, 0);
	sem_t *semCustomerWait = sem_open(SEMCUSTOMERWAIT, 0);
	sem_t *semBar = sem_open(SEMBAR, 0);
	sem_t *semStats = sem_open(SEMSTATS, 0);
	sem_close(semDoorQueue);
	sem_unlink(SEMDOORQUEUE);
	sem_close(semDoormanWait1);
	sem_unlink(SEMDOORMANWAIT1);
	sem_close(semDoormanWait2);
	sem_unlink(SEMDOORMANWAIT2);
	sem_close(semDoormanWait3);
	sem_unlink(SEMDOORMANWAIT3);
	sem_close(semCustomerWait);
	sem_unlink(SEMCUSTOMERWAIT);
	sem_close(semBar);
	sem_unlink(SEMBAR);
	sem_close(semStats);
	sem_unlink(SEMSTATS);
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
