/*
 * node.h
 *
 *  Created on: Mar 3, 2016
 *      Author: vangelis
 */

#ifndef NODE_H_
#define NODE_H_

#include "edge.h"

typedef struct Node{
	int id;
	int visited;
	int sizeOfOut;
	int sizeOfIn;
	int numberOfInEdges;
	int numberOfOutEdges;
	Edge* pointerToIn;
	Edge* pointerToOut;
}Node;

#endif /* NODE_H_ */
