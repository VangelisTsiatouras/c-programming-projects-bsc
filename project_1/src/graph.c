/*
 * graph.c
 *
 *  Created on: Mar 15, 2016
 *      Author: vangelis
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"

void AddNode(HashTable* hashTable, int nodeID) {
	int i = 0, j, stored = 0;
	int bucketNumber = HashFunction(nodeID, hashTable->size);
	if (SearchNode(hashTable, nodeID) >= 0) {
		printf("failure: %d already exists!\n", nodeID);
		return;
	}
	//If the node array of the bucket is full then reallocate
	if (hashTable->nodeArray[bucketNumber].storedNodes == hashTable->nodeArray[bucketNumber].sizeOfBucket) {
		int doubleSize = 2 * hashTable->nodeArray[bucketNumber].sizeOfBucket;
		hashTable->nodeArray[bucketNumber].node = (Node *) realloc(hashTable->nodeArray[bucketNumber].node,
				sizeof(Node) * doubleSize);
		if (hashTable->nodeArray[bucketNumber].node == NULL) {
			printf("REALLOC ERROR\n");
			return;
		}
		//Reinitialize the node array
		for (j = 0; j < doubleSize; j++) {
			if (j >= hashTable->nodeArray[bucketNumber].sizeOfBucket) {
				hashTable->nodeArray[bucketNumber].node[j].id = -1;
				hashTable->nodeArray[bucketNumber].node[j].visited = 0;
				hashTable->nodeArray[bucketNumber].node[j].sizeOfIn = 4;
				hashTable->nodeArray[bucketNumber].node[j].sizeOfOut = 4;
				hashTable->nodeArray[bucketNumber].node[j].pointerToIn = (Edge*) malloc(sizeof(Edge) * 4);
				if (hashTable->nodeArray[bucketNumber].node[j].pointerToIn == NULL) {
					printf("REALLOC ERROR!\n");
					return;
				}
				hashTable->nodeArray[bucketNumber].node[j].pointerToOut = (Edge*) malloc(sizeof(Edge) * 4);
				if (hashTable->nodeArray[bucketNumber].node[j].pointerToOut == NULL) {
					printf("REALLOC ERROR!\n");
					return;
				}
			}
		}
		hashTable->nodeArray[bucketNumber].sizeOfBucket = doubleSize;
	}
	while (i < hashTable->nodeArray[bucketNumber].sizeOfBucket && stored == 0) {
		//Find empty position in bucket array
		if (hashTable->nodeArray[bucketNumber].node[i].id == -1) {
			//Initialize the new node
			int j;
			hashTable->nodeArray[bucketNumber].node[i].id = nodeID;
			hashTable->nodeArray[bucketNumber].node[i].visited = 0;
			hashTable->nodeArray[bucketNumber].node[i].numberOfInEdges = 0;
			hashTable->nodeArray[bucketNumber].node[i].numberOfOutEdges = 0;
			hashTable->nodeArray[bucketNumber].node[i].sizeOfIn = 4;
			hashTable->nodeArray[bucketNumber].node[i].sizeOfOut = 4;
			for (j = 0; j < 4; j++) {
				hashTable->nodeArray[bucketNumber].node[i].pointerToOut[j].id = -1;
				hashTable->nodeArray[bucketNumber].node[i].pointerToOut[j].transaction = 0;
			}
			for (j = 0; j < 4; j++) {
				hashTable->nodeArray[bucketNumber].node[i].pointerToIn[j].id = -1;
				hashTable->nodeArray[bucketNumber].node[i].pointerToIn[j].transaction = 0;
			}
			hashTable->storedNodes++;
			hashTable->nodeArray[bucketNumber].storedNodes++;
			stored = 1;
			printf("success: created %d\n", nodeID);
		}
		i++;
	}
}

void DeleteNode(HashTable* hashTable, int nodeID) {
	int i = 0;
	int bucketNumber = HashFunction(nodeID, hashTable->size);
	int positionInBucket = SearchNode(hashTable, nodeID);
	if (positionInBucket == -1) {
		printf("failure: %d does not exist\n", nodeID);
		return;
	}
	//Search the out node array
	for (i = 0; i < hashTable->nodeArray[bucketNumber].node[positionInBucket].numberOfOutEdges; i++) {
		if (hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToOut[i].id != -1) {
			printf("failure: %d cannot be deleted! To remove it, delete all the transactions that involve %d\n", nodeID,
					nodeID);
			return;
		}
	}
	//Search the in node array
	for (i = 0; i < hashTable->nodeArray[bucketNumber].node[positionInBucket].numberOfInEdges; i++) {
		if (hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToIn[i].id != -1) {
			printf("failure: %d cannot be deleted! To remove it, delete all the transactions that involve %d\n", nodeID,
					nodeID);
			return;
		}
	}
	//Delete the node
	free(hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToOut);
	free(hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToIn);
	hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToOut = malloc(sizeof(Edge) * 4);
	if (hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToOut == NULL) {
		printf("MALLOC ERROR\n");
		return;
	}
	hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToIn = malloc(sizeof(Edge) * 4);
	if (hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToIn == NULL) {
		printf("MALLOC ERROR\n");
		return;
	}
	hashTable->nodeArray[bucketNumber].node[positionInBucket].id = -1;
	hashTable->nodeArray[bucketNumber].node[positionInBucket].numberOfInEdges = 0;
	hashTable->nodeArray[bucketNumber].node[positionInBucket].numberOfOutEdges = 0;
	hashTable->nodeArray[bucketNumber].node[positionInBucket].sizeOfIn = 4;
	hashTable->nodeArray[bucketNumber].node[positionInBucket].sizeOfOut = 4;
	hashTable->nodeArray[bucketNumber].storedNodes--;
	hashTable->storedNodes--;
	printf("success: deleted %d\n", nodeID);
}

void AddTran(HashTable* hashTable, int fromNodeID, int toNodeID, float amount) {
	int i = 0, foundOutNode = 0, foundInNode = 0;
	if (fromNodeID == toNodeID) {
		printf("failure: in and out nodes must be different\n");
		return;
	}
	if (amount < 0) {
		printf("failure: amount cannot be negative\n");
		return;
	}
	int fromNodeIDBucketNumber = HashFunction(fromNodeID, hashTable->size);
	int toNodeIDBucketNumber = HashFunction(toNodeID, hashTable->size);
	int fromNodeIDPositionInBucket = SearchNode(hashTable, fromNodeID);
	int toNodeIDPositionInBucket = SearchNode(hashTable, toNodeID);
	if (fromNodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist\n", fromNodeID);
		return;
	}
	if (toNodeIDPositionInBucket == -1) {
		printf("failure: %d does not exist\n", toNodeID);
		return;
	}
	//Go to the bucket where is stored the node which sends the amount
	//Find the destination node in pointerToOut array
	while (i < hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut
			&& foundOutNode == 0) {
		if (hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].id
				== toNodeID) {
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].transaction +=
					amount;
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].numberOfOutEdges++;
			foundOutNode = 1;
		}
		i++;
	}
	//If destination node wasn't found create new edge between the nodes
	if (foundOutNode == 0) {
		//If the out edge array is full then reallocate
		if (hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut
				== hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].numberOfOutEdges) {
			int doubleSize = hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut
					* 2;
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut = realloc(
					hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut,
					sizeof(Node) * doubleSize);
			if (hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut == NULL) {
				printf("REALLOC ERROR!\n");
				return;
			}
			for (i = 0; i < doubleSize; i++) {
				if (i >= hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut) {
					hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].id =
							-1;
				}
			}
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut = doubleSize;
		}
		int connection = 0;
		i = 0;
		//Find an empty position in pointerToOut array and store the nodeID which receives the amount
		while (i < hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut
				&& connection == 0) {
			if (hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].id
					== -1) {
				hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].id =
						toNodeID;
				hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].transaction =
						amount;
				hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].numberOfOutEdges++;
				connection = 1;
			}
			i++;
		}
	}
	//Go to the bucket where is stored the node which receives the amount
	//And do the same thing......
	//Find the destination node in pointerToIn array
	i = 0;
	while (i < hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn && foundInNode == 0) {
		if (hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].id == fromNodeID) {
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].transaction +=
					amount;
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].numberOfInEdges++;
			foundInNode = 1;
		}
		i++;
	}
	//If destination node wasn't found create new edge between the nodes
	if (foundInNode == 0) {
		//If the out edge array is full then reallocate
		if (hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn
				== hashTable->nodeArray[fromNodeIDBucketNumber].node[toNodeIDPositionInBucket].numberOfInEdges) {
			int doubleSize = hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn * 2;
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn = realloc(
					hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn,
					sizeof(Node) * doubleSize);
			if (hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn == NULL) {
				printf("REALLOC ERROR!\n");
				return;
			}
			for (i = 0; i < doubleSize; i++) {
				if (i >= hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn) {
					hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].id = -1;
				}
			}
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn = doubleSize;
		}
		int connection = 0;
		i = 0;
		//Find an empty position in pointerToIn array and store the nodeID which sends the amount
		while (i < hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn && connection == 0) {
			if (hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].id == -1) {
				hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].id =
						fromNodeID;
				hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].transaction =
						amount;
				hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].numberOfInEdges++;
				connection = 1;
			}
			i++;
		}
	}
	printf("success: added transaction %d %d with %f\n", fromNodeID, toNodeID, amount);
}

void DeleteTran(HashTable* hashTable, int fromNodeID, int toNodeID) {
	int i = 0, foundOutNode = 0, foundInNode = 0;
	if (fromNodeID == toNodeID) {
		printf("failure: in and out nodes must be different\n");
		return;
	}
	int fromNodeIDBucketNumber = HashFunction(fromNodeID, hashTable->size);
	int toNodeIDBucketNumber = HashFunction(toNodeID, hashTable->size);
	int fromNodeIDPositionInBucket = SearchNode(hashTable, fromNodeID);
	int toNodeIDPositionInBucket = SearchNode(hashTable, toNodeID);
	if (fromNodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist\n", fromNodeID);
		return;
	}
	if (toNodeIDPositionInBucket == -1) {
		printf("failure: %d does not exist\n", toNodeID);
		return;
	}
	//Go to the bucket where is stored the node which sends the amount
	//Find the destination node in pointerToOut array
	while (i < hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].sizeOfOut
			&& foundOutNode == 0) {
		if (hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].id
				== toNodeID) {
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].id = -1;
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].pointerToOut[i].transaction =
					0;
			hashTable->nodeArray[fromNodeIDBucketNumber].node[fromNodeIDPositionInBucket].numberOfOutEdges--;
			foundOutNode = 1;
		}
		i++;
	}
	if (foundOutNode == 0) {
		printf("failure: %d does not have a transaction with %d\n", fromNodeID, toNodeID);
	}
	//Go to the bucket where is stored the node which receives the amount
	//Find the destination node in pointerToOut array
	while (i < hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].sizeOfIn && foundInNode == 0) {
		if (hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].id == fromNodeID) {
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].id = -1;
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].pointerToIn[i].transaction = 0;
			hashTable->nodeArray[toNodeIDBucketNumber].node[toNodeIDPositionInBucket].numberOfInEdges--;
			foundInNode = 1;
		}
		i++;
	}
	printf("success: deleted transaction %d %d\n", fromNodeID, toNodeID);
}

void Lookup(HashTable* hashTable, int nodeID, char* option) {
	int nodeIDBucketNumber = HashFunction(nodeID, hashTable->size);
	int nodeIDPositionInBucket = SearchNode(hashTable, nodeID);
	if (nodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist\n", nodeID);
		return;
	}
	if (strcmp(option, "in") == 0) {
		printf("success: in(%d) = %f\n", nodeID,
				SumAmountInNodes(hashTable, nodeIDBucketNumber, nodeIDPositionInBucket));
	} else if (strcmp(option, "out") == 0) {
		printf("success: out(%d) = %f\n", nodeID,
				SumAmountOutNodes(hashTable, nodeIDBucketNumber, nodeIDPositionInBucket));
	} else if (strcmp(option, "sum") == 0) {
		printf("success: sum(%d) = %f\n", nodeID,
				SumAmountInNodes(hashTable, nodeIDBucketNumber, nodeIDPositionInBucket)
						- SumAmountOutNodes(hashTable, nodeIDBucketNumber, nodeIDPositionInBucket));
	} else {
		printf("failure: unknown option\n");
		return;
	}
}

float SumAmountOutNodes(HashTable* hashTable, int bucketNumber, int positionInBucket) {
	float sum = 0;
	int i;
	for (i = 0; i < hashTable->nodeArray[bucketNumber].node[positionInBucket].sizeOfOut; i++) {
		if (hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToOut[i].id != -1) {
			sum += hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToOut[i].transaction;
		}
	}
	return sum;
}

float SumAmountInNodes(HashTable* hashTable, int bucketNumber, int positionInBucket) {
	float sum = 0;
	int i;
	for (i = 0; i < hashTable->nodeArray[bucketNumber].node[positionInBucket].sizeOfIn; i++) {
		if (hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToIn[i].id != -1) {
			sum += hashTable->nodeArray[bucketNumber].node[positionInBucket].pointerToIn[i].transaction;
		}
	}
	return sum;
}

void Triangle(HashTable* hashTable, int nodeID, float amount) {
	//Implementation with 3 nested loops
	int i;
	int triangleFound = 0;
	int nodeIDBucketNumber = HashFunction(nodeID, hashTable->size);
	int nodeIDPositionInBucket = SearchNode(hashTable, nodeID);
	if (nodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist\n", nodeID);
		return;
	}
	if (amount < 0) {
		printf("amount cannot be negative\n");
		return;
	}
	//Search the out node array of nodeId
	for (i = 0; i < hashTable->nodeArray[nodeIDBucketNumber].node[nodeIDPositionInBucket].sizeOfOut; i++) {
		//If the transaction between the nodes is greater than or equal with the amount we give continue searching
		if (hashTable->nodeArray[nodeIDBucketNumber].node[nodeIDPositionInBucket].pointerToOut[i].transaction
				>= amount) {
			int j;
			int tempNode1 = hashTable->nodeArray[nodeIDBucketNumber].node[nodeIDPositionInBucket].pointerToOut[i].id;
			int tempNode1BucketNumber = HashFunction(tempNode1, hashTable->size);
			int tempNode1PositionInBucket = SearchNode(hashTable, tempNode1);
			//Search the out node array of tempNode1
			for (j = 0; j < hashTable->nodeArray[tempNode1BucketNumber].node[tempNode1PositionInBucket].sizeOfOut;
					j++) {
				if (hashTable->nodeArray[tempNode1BucketNumber].node[tempNode1PositionInBucket].pointerToOut[j].transaction
						>= amount) {
					int z;
					int tempNode2 =
							hashTable->nodeArray[tempNode1BucketNumber].node[tempNode1PositionInBucket].pointerToOut[j].id;
					int tempNode2BucketNumber = HashFunction(tempNode2, hashTable->size);
					int tempNode2PositionInBucket = SearchNode(hashTable, tempNode2);
					//Search the out node array of tempNode2
					for (z = 0;
							z < hashTable->nodeArray[tempNode2BucketNumber].node[tempNode2PositionInBucket].sizeOfOut;
							z++) {
						if (hashTable->nodeArray[tempNode2BucketNumber].node[tempNode2PositionInBucket].pointerToOut[z].transaction
								>= amount
								&& hashTable->nodeArray[tempNode2BucketNumber].node[tempNode2PositionInBucket].pointerToOut[z].id
										== nodeID) {
							//If this is the first triangle we find
							if (triangleFound == 0) {
								printf("success: triangle(%d,%f) =\n", nodeID, amount);
							}
							triangleFound = 1;
							printf("(%d,%d,%d)\n", nodeID, tempNode1, tempNode2);
						}
					}
				}
			}
		}
	}
	//If no triangles found then print failure message
	if (triangleFound == 0) {
		printf("failure: no triangles found for %d with amount greater than %f!\n", nodeID, amount);
	}
}

void Connection(HashTable* hashTable, int fromNodeID, int toNodeID) {
	//Implementation with recurse
	int i;
	if (fromNodeID == toNodeID) {
		printf("failure: in and out nodes must be different\n");
		return;
	}
	int fromNodeIDBucketNumber = HashFunction(fromNodeID, hashTable->size);
	int fromNodeIDPositionInBucket = SearchNode(hashTable, fromNodeID);
	if (fromNodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist\n", fromNodeID);
		return;
	}
	int pathArray[hashTable->storedNodes];
	for (i = 0; i < hashTable->storedNodes; i++) {
		pathArray[i] = -1;
	}
	SetAllNodesUnvisited(hashTable);
	int connectionFound = FindPath(hashTable, fromNodeIDBucketNumber, fromNodeIDPositionInBucket, pathArray, 0,
			fromNodeID, toNodeID, 0);
	if (connectionFound == 0) {
		printf("conn(%d,%d) not found\n", fromNodeID, toNodeID);
	}
}

int FindPath(HashTable* hashTable, int curentNodeIDBucketNumber, int curentNodeIDPositionInBucket, int* pathArray,
		int numberOfNodesInPath, int startNodeID, int destNodeID, int pathFound) {
	int i, j;
	int currentID = hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].id;
	//Set currentNodeID as visited
	hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].visited = 1;
	pathArray[numberOfNodesInPath] = currentID;
	numberOfNodesInPath++;
	//Check if currentNodeID is the destination we want
	if (currentID == destNodeID) {
		int comma = 0;
		printf("success: conn(%d,%d) = (", startNodeID, destNodeID);
		for (j = 0; j < hashTable->storedNodes; j++) {
			if (pathArray[j] != -1) {
				if (comma == 1) {
					printf(",");
				}
				printf("%d", pathArray[j]);
				comma = 1;
			}
		}
		printf(")\n");
		pathFound = 1;
		return pathFound;
	}
	//Search the out node array of currentNodeID
	int fetchedNodeID, fetchedNodeBucketNumber, fetchedNodePositionInBucket;
	for (i = 0; i < hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].sizeOfOut; i++) {
		//Check if node is unvisited
		fetchedNodeID =
				hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].pointerToOut[i].id;
		if (fetchedNodeID != -1) {
			fetchedNodeBucketNumber = HashFunction(fetchedNodeID, hashTable->size);
			fetchedNodePositionInBucket = SearchNode(hashTable, fetchedNodeID);
			if (hashTable->nodeArray[fetchedNodeBucketNumber].node[fetchedNodePositionInBucket].visited == 0) {
				//Recur
				pathFound = FindPath(hashTable, fetchedNodeBucketNumber, fetchedNodePositionInBucket, pathArray,
						numberOfNodesInPath, startNodeID, destNodeID, pathFound);
			}
		}
	}
	pathArray[numberOfNodesInPath] = -1;
	numberOfNodesInPath--;
	return pathFound;
}

void AllCycles(HashTable* hashTable, int nodeID) {
	//Implementation with recurse
	int i;
	int nodeIDBucketNumber = HashFunction(nodeID, hashTable->size);
	int nodeIDPositionInBucket = SearchNode(hashTable, nodeID);
	if (nodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist!\n", nodeID);
		return;
	}
	int pathArray[hashTable->storedNodes];
	for (i = 0; i < hashTable->storedNodes; i++) {
		pathArray[i] = -1;
	}
	SetAllNodesUnvisited(hashTable);
	int cycleFound = FindCycle(hashTable, nodeIDBucketNumber, nodeIDPositionInBucket, pathArray, 0, nodeID, 0, 0);
	if (cycleFound == 0) {
		printf("cycle not found for %d\n", nodeID);
	}
}

int FindCycle(HashTable* hashTable, int curentNodeIDBucketNumber, int curentNodeIDPositionInBucket, int* pathArray,
		int numberOfNodesInPath, int startNodeID, int firstCall, int cycleFound) {
	int i, j;
	int currentID = hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].id;
	//Set currentNodeID as visited
	hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].visited = 1;
	pathArray[numberOfNodesInPath] = currentID;
	numberOfNodesInPath++;
	//Check if currentNodeID is the destination we want
	if (currentID == startNodeID && firstCall != 0) {
		if (numberOfNodesInPath > 4) {
			int comma = 0;
			if (cycleFound == 0) {
				printf("success: cycles(%d) =\n", startNodeID);

			}
			//Print the startNode
			printf("(%d,", pathArray[0]);
			for (j = 0; j < hashTable->storedNodes; j++) {
				if (pathArray[j] != -1 && pathArray[j] != startNodeID) {
					if (comma == 1) {
						printf(",");
					}
					printf("%d", pathArray[j]);
					comma = 1;
				}
			}
			printf(")\n");
			cycleFound = 1;
			firstCall = 1;
			return cycleFound;
		} else {
			return cycleFound;
		}
	}
	//Search the out node array of currentNodeID
	int fetchedNodeID, fetchedNodeBucketNumber, fetchedNodePositionInBucket;
	for (i = 0; i < hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].sizeOfOut; i++) {
		//Check if node is unvisited
		fetchedNodeID =
				hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].pointerToOut[i].id;
		if (fetchedNodeID != -1) {
			fetchedNodeBucketNumber = HashFunction(fetchedNodeID, hashTable->size);
			fetchedNodePositionInBucket = SearchNode(hashTable, fetchedNodeID);
			if (hashTable->nodeArray[fetchedNodeBucketNumber].node[fetchedNodePositionInBucket].visited == 0
					|| fetchedNodeID == startNodeID) {
				//Recur
				cycleFound = FindCycle(hashTable, fetchedNodeBucketNumber, fetchedNodePositionInBucket, pathArray,
						numberOfNodesInPath, startNodeID, 1, cycleFound);
				//If we found a circle with less than 4 nodes set this node as unvisited
				if (cycleFound == 0 && fetchedNodeID == startNodeID) {
					hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].visited = 0;
				}
			}
		}
	}
	pathArray[numberOfNodesInPath] = -1;
	numberOfNodesInPath--;
	return cycleFound;
}

void Traceflow(HashTable* hashTable, int nodeID, int depth) {
	//Implementation with recurse
	int i;
	int nodeIDBucketNumber = HashFunction(nodeID, hashTable->size);
	int nodeIDPositionInBucket = SearchNode(hashTable, nodeID);
	if (nodeIDPositionInBucket < 0) {
		printf("failure: %d does not exist\n", nodeID);
		return;
	}
	if (depth <= 0) {
		printf("failure: amount cannot be negative or equal to 0\n");
		return;
	}
	int pathArray[hashTable->storedNodes];
	for (i = 0; i < hashTable->storedNodes; i++) {
		pathArray[i] = -1;
	}
	SetAllNodesUnvisited(hashTable);
	int traceFound = FindTrace(hashTable, nodeIDBucketNumber, nodeIDPositionInBucket, pathArray, 0, 0, nodeID, depth,
			0);
	if (traceFound == 0) {
		printf("failure trace not found for %d in depth %d\n", nodeID, depth);
	}
}

int FindTrace(HashTable* hashTable, int curentNodeIDBucketNumber, int curentNodeIDPositionInBucket, int* pathArray,
		int numberOfNodesInPath, float amount, int startNodeID, int depth, int traceFound) {
	int i, j;
	int currentID = hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].id;
	//Set currentNodeID as visited
	hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].visited = 1;
	pathArray[numberOfNodesInPath] = currentID;
	numberOfNodesInPath++;
	//Check if currentNodeID is the destination we want
	if (numberOfNodesInPath == (depth + 1)) {
		if (numberOfNodesInPath >= 2) {
			int comma = 0;
			if (traceFound == 0) {
				printf("success: traceflow(%d, %d) =\n", startNodeID, depth);
			}
			printf("(");
			for (j = 0; j < hashTable->storedNodes; j++) {
				if (pathArray[j] != -1) {
					if (comma == 1) {
						printf(",");
					}
					printf("%d", pathArray[j]);
					comma = 1;
				};
			}
			printf(", %f)\n", amount);
			traceFound = 1;
			return traceFound;
		}
	}
	//Search the out node array of currentNodeID
	int fetchedNodeID = 0, fetchedNodeBucketNumber, fetchedNodePositionInBucket;
	for (i = 0; i < hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].sizeOfOut; i++) {
		//Check if node is unvisited
		fetchedNodeID =
				hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].pointerToOut[i].id;
		if (fetchedNodeID != -1) {
			fetchedNodeBucketNumber = HashFunction(fetchedNodeID, hashTable->size);
			fetchedNodePositionInBucket = SearchNode(hashTable, fetchedNodeID);
			if (hashTable->nodeArray[fetchedNodeBucketNumber].node[fetchedNodePositionInBucket].visited == 0) {
				amount +=
						hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].pointerToOut[i].transaction;
				//Recur
				traceFound = FindTrace(hashTable, fetchedNodeBucketNumber, fetchedNodePositionInBucket, pathArray,
						numberOfNodesInPath, amount, startNodeID, depth, traceFound);
			}
		}
	}
	amount -=
			hashTable->nodeArray[curentNodeIDBucketNumber].node[curentNodeIDPositionInBucket].pointerToOut[i].transaction;
	pathArray[numberOfNodesInPath] = -1;
	numberOfNodesInPath--;
	return traceFound;
}

void Bye(HashTable* hashTable) {
	int i, j;
	for (i = 0; i < hashTable->size; i++) {
		//Go to the buckets of the hashtable
		for (j = 0; j < hashTable->nodeArray[i].sizeOfBucket; j++) {
			//Go to the nodes
			if (hashTable->nodeArray[i].node[j].id != -1) {
				free(hashTable->nodeArray[i].node[j].pointerToOut);
				free(hashTable->nodeArray[i].node[j].pointerToIn);
			}
		}
		free(hashTable->nodeArray[i].node);
	}
	hashTable->storedNodes = 0;
	//REINITIALIZE
	//Allocate space for 4 nodes per bucket
	for (i = 0; i < hashTable->size; i++) {
		//Initialization
		hashTable->nodeArray[i].sizeOfBucket = 4;
		hashTable->nodeArray[i].storedNodes = 0;
		hashTable->nodeArray[i].node = (Node *) malloc(sizeof(Node) * 4);
		if (hashTable->nodeArray[i].node == NULL) {
			printf("MALLOC ERROR!\n");
			return;
		}
		for (j = 0; j < hashTable->nodeArray[i].sizeOfBucket; j++) {
			hashTable->nodeArray[i].node[j].id = -1;
			hashTable->nodeArray[i].node[j].visited = 0;
		}
	}
	printf("success: cleaned memory\n");
}

void PrintHash(HashTable* hashTable) {
	int i, j, z;
	//Flag to not print comma after "vertex(N) = "
	int comma = 0;
	if (hashTable->storedNodes == 0) {
		printf("failure: there aren't any nodes stored\n");
	}
	for (i = 0; i < hashTable->size; i++) {
		for (j = 0; j < hashTable->nodeArray[i].sizeOfBucket; j++) {
			if (hashTable->nodeArray[i].node[j].id != -1) {
				printf("vertex(%d) = ", hashTable->nodeArray[i].node[j].id);
				comma = 0;
				for (z = 0; z < hashTable->nodeArray[i].node[j].sizeOfOut; z++) {
					if (hashTable->nodeArray[i].node[j].pointerToOut[z].id != -1) {
						if (comma == 1) {
							printf(",");
						}
						printf("(%d,%f) ", hashTable->nodeArray[i].node[j].pointerToOut[z].id,
								hashTable->nodeArray[i].node[j].pointerToOut[z].transaction);
						comma = 1;
					}
				}
				printf("\n");
			}
		}
	}
}

int SearchNode(HashTable* hashTable, int nodeID) {
	int i = 0;
	int bucketNumber = HashFunction(nodeID, hashTable->size);
	int positionInBucket = -1;
	while (i < hashTable->nodeArray[bucketNumber].sizeOfBucket && positionInBucket == -1) {
		if (hashTable->nodeArray[bucketNumber].node[i].id == nodeID) {
			positionInBucket = i;
		}
		i++;
	}
	//Return -1 if the searched node does not exists otherwise return its position into the bucket
	return positionInBucket;
}

void SetAllNodesUnvisited(HashTable* hashTable) {
	int i, j;
	for (i = 0; i < hashTable->size; i++) {
		for (j = 0; j < hashTable->nodeArray[i].sizeOfBucket; j++) {
			if (hashTable->nodeArray[i].node[j].visited == 1) {
				hashTable->nodeArray[i].node[j].visited = 0;
			}
		}
	}
}
