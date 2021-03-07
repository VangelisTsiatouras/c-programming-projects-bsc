/*
 * dinodes_structure.h
 *
 *  Created on: Jan 19, 2017
 *      Author: vangelis
 */

#ifndef DINODES_STRUCTURE_H_
#define DINODES_STRUCTURE_H_

#include <sys/stat.h>
#include "definitions.h"

typedef struct DinodeElement {
	char name[NAME_SIZE];
	int positionInList;
} DinodeElement;

typedef struct DInodeList {
	char name[NAME_SIZE];
	int permissions;
	int size;
	time_t atime; /* time of last access */
	time_t mtime; /* time of last modification */
	time_t ctime; /* time of last status change */
	int flagDirectory;
	int ptrToFile;
	DinodeElement dinode[DINODES_PER_DIRECTORY];
} DInodeList;

#endif /* DINODES_STRUCTURE_H_ */
