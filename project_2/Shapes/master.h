/*
 * master.h
 *
 *  Created on: Nov 5, 2016
 *      Author: vangelis
 */

#ifndef MASTER_H_
#define MASTER_H_

void Master(char *inputBinaryFile, int workersCount, char *tempDir);

int ExecuteCommands(char *buf, char *inputBinaryFile, int workersCount, char *tempDir, int *offsets, int *pointsToRead, int completedCommands);

#endif /* MASTER_H_ */
