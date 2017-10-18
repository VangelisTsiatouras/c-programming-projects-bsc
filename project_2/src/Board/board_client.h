/*
 * board_client.h
 *
 *  Created on: Apr 13, 2016
 *      Author: vangelis
 */

#ifndef BOARD_CLIENT_H_
#define BOARD_CLIENT_H_

void BoardClientShell(char *path, char *boardName);

int BoardClientCommands(char *lineBuffer, char *path, char *boardName);

void BoardClientCreatechannel(char *path , int id, char *name, char *boardName);

void BoardClientGetMessages(char *path , int id, char *boardName);

void BoardClientShutdown(char *path, char *boardName);

#endif /* BOARD_CLIENT_H_ */
