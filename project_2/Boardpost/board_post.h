/*
 * board_post.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vangelis
 */

#ifndef BOARD_POST_H_
#define BOARD_POST_H_

void BoardPostShell(char *path, char *boardName);

int BoardPostCommands(char *lineBuffer, char *path, char *boardName);

void BoardPostList(char *path, char *boardName);

void BoardPostWrite(int id, char *message, char *path, char *boardName);

void BoardPostSend(int id, char *pathOfFile, char *path, char *boardName);

char* Concatenate(char *path, char *boardName, char *filename);

#endif /* BOARD_POST_H_ */
