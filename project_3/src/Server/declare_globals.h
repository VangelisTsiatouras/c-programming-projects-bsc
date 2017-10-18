/*
 * declare_globals.h
 *
 *  Created on: May 26, 2016
 *      Author: vangelis
 */

#ifndef DECLARE_GLOBALS_H_
#define DECLARE_GLOBALS_H_

extern HashTable hashTable;
extern ThreadPool threadPool;

extern pthread_mutex_t mtx;
extern pthread_cond_t cond_nonempty;
extern pthread_cond_t cond_nonfull;

#endif /* DECLARE_GLOBALS_H_ */
