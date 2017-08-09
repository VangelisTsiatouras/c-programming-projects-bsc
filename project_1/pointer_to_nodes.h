/*
 * pointer_to_nodes.h
 *
 *  Created on: Mar 8, 2016
 *      Author: vangelis
 */

#ifndef POINTER_TO_NODES_H_
#define POINTER_TO_NODES_H_

#include "node.h"

typedef struct PtrToNodes{
	int sizeOfBucket;
	int storedNodes;
	Node* node;
}PtrToNodes;

#endif /* POINTER_TO_NODES_H_ */
