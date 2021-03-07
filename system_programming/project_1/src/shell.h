/*
 * shell.h
 *
 *  Created on: Mar 15, 2016
 *      Author: vangelis
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "graph.h"

void shell(int numberOfBuckets, char* operationFile);

void ShellExecuteCommand(char* lineBuffer, HashTable* hashTable);

#endif /* SHELL_H_ */
