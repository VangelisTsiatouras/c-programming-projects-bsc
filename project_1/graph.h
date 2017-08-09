/*
 * graph.h
 *
 *  Created on: Mar 15, 2016
 *      Author: vangelis
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "hashtable.h"

void AddNode(HashTable* hashTable, int nodeID);

void DeleteNode(HashTable* hashTable, int nodeID);

void AddTran(HashTable* hashTable, int fromNodeID, int toNodeID, float amount);

void DeleteTran(HashTable* hashTable, int fromNodeID, int toNodeID);

void Lookup(HashTable* hashTable, int nodeID, char* option);

float SumAmountOutNodes(HashTable* hashTable, int bucketNumber, int positionInBucket);

float SumAmountInNodes(HashTable* hashTable, int bucketNumber, int positionInBucket);

void Triangle(HashTable* hashTable, int nodeID, float amount);

void Connection(HashTable* hashTable, int fromNodeID, int toNodeID);

int FindPath(HashTable* hashTable, int curentNodeIDBucketNumber, int curentNodeIDPositionInBucket, int* pathArray,
		int numberOfNodesInPath, int startNodeID, int destNodeID, int pathFound);

void AllCycles(HashTable* hashTable, int nodeID);

int FindCycle(HashTable* hashTable, int curentNodeIDBucketNumber, int curentNodeIDPositionInBucket, int* pathArray,
		int numberOfNodesInPath, int nodeID, int firstCall, int cycleFound);

void Traceflow(HashTable* hashTable, int nodeID, int depth);

int FindTrace(HashTable* hashTable, int curentNodeIDBucketNumber, int curentNodeIDPositionInBucket, int* pathArray,
		int numberOfNodesInPath, float amount, int startNodeID, int depth, int traceFound);

void Bye(HashTable* hashTable);

void PrintHash(HashTable* hashTable);

int SearchNode(HashTable* hashTable, int nodeID);

void SetAllNodesUnvisited(HashTable* hashTable);

#endif /* GRAPH_H_ */
