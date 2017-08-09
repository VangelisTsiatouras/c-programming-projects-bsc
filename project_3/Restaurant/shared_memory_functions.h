/*
 * shared_memory_functions.h
 *
 *  Created on: Dec 3, 2016
 *      Author: vangelis
 */

#ifndef SHARED_MEMORY_FUNCTIONS_H_
#define SHARED_MEMORY_FUNCTIONS_H_

int SharedMemInit(int numberOfTables, int maxSizeOfTables, int numberOfWaiters, int numberOfCustomers);

int SharedMemPurge(int shmid);

#endif /* SHARED_MEMORY_FUNCTIONS_H_ */
