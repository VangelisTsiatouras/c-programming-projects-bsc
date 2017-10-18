/*
 * channel.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vangelis
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

/* Messages are stored in a dynamic array.
 * The files will be stored in the hard disk
 * Only the file descriptors are stored in a
 * dynamic array.
*/

typedef struct Channel {
	int id;
	char name[64];
	int maxMessages;
	int numberOfMessages;
	char **messages;
	int maxFiles;
	int numberOfFiles;
	FILE **filesFDs;
} Channel;

#endif /* CHANNEL_H_ */
