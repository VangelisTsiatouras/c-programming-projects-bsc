/*
 * di_header.h
 *
 *  Created on: Feb 21, 2017
 *      Author: vangelis
 */

#ifndef DI_HEADER_H_
#define DI_HEADER_H_

#include "definitions.h"

typedef struct Header {
	char identifier[16]; //I AM A DI FILE
	int totalSize;
	int pointerToMeta;
	int blocksOfMeta;
} Header;

#endif /* DI_HEADER_H_ */
