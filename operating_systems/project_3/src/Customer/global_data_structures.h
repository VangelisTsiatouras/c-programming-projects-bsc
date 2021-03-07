/*
 * global_data_structures.h
 *
 *  Created on: Dec 10, 2016
 *      Author: vangelis
 */

#ifndef GLOBAL_DATA_STRUCTURES_H_
#define GLOBAL_DATA_STRUCTURES_H_


/* Semaphore name definitions*/
/* Door queue semaphore*/
#define SEMDOORQUEUE "/semaphore_door_queue"
/* Semaphore that locks doorman until customer defines its group size at the door*/
#define SEMDOORMANWAIT1 "/semaphore_doorman_wait_1"
/* Semaphore that locks doorman until customer sits on table or sits at bar or leaves*/
#define SEMDOORMANWAIT2 "/semaphore_doorman_wait_2"
/* Semaphore that locks doorman until customer (who was at the bar!!!) sits on table */
#define SEMDOORMANWAIT3 "/semaphore_doorman_wait_3"
/* Semaphore that locks customer at the door*/
#define SEMCUSTOMERWAIT "/semaphore_customer_wait"
/* Semaphore that locks the bar*/
#define SEMBAR "/semaphore_bar"
/* Semaphore that locks the stats of the restaurant*/
#define SEMSTATS "/semaphore_stats"

/* Restaurant Structure*/

/* This structure contains useful info about customers
 * that are waiting at the bar
 */
typedef struct CustomerGroup {
	pid_t groupPID;
	int groupSize;
	int positionBar;
	int gotoTableID;
} CustomerGroup;

/* This structure is used for the communication between
 * the customer and the doorman at the door of the restaurant
 */
typedef struct Door {
	int customerGroupSize;
	pid_t groupPID;
	int gotoBar;
	int gotoTableID;
	int leave;
} Door;

/* The bar is implemented with the help of a static array
 */
typedef struct Bar {
	int capacity;
	int occupiedSeats;
	CustomerGroup groupsWaitingAtBar[15];
} Bar;

typedef struct Table {
	sem_t semCustomerWait;
	sem_t semWaiterWait;
	pid_t groupPID;
	int occupied;
	int size;
	int readyToOrder;
	int readyToPay;
} Table;

typedef struct Stats {
	int targetNumberOfCustomers;
	int totalAmount;
	int tablesFull;
	int totalCustomersArrived;
	int customersLeft;
	int customersServiced;
} Stats;

typedef struct RestaurantStruct {
	Stats stats;
	Door door;
	Bar bar;
	int numberOfWaiters;
	int numberOfTables;
	Table tables[];
} RestaurantStruct;

#endif /* GLOBAL_DATA_STRUCTURES_H_ */
