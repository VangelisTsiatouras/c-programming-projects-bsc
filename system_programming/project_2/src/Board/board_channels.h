/*
 * board_channels.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vangelis
 */

#ifndef BOARD_CHANNELS_H_
#define BOARD_CHANNELS_H_

#include "channel.h"

/* All the channels are stored in a dynamic array.*/

typedef struct BoardChannels {
	int numberOfActiveChannels;
	int maxActiveChannels;
	Channel *channelArray;
} BoardChannels;

#endif /* BOARD_CHANNELS_H_ */
