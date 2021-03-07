/*
 * main.c
 *
 *  Created on: Dec 3, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "doorman_routines.h"

int main(int argc, char **argv) {
	int i, minusD = 0, minusS = 0, NOperiod = 0, NOshmid = 0;
	int period, shmid;
	/* Split args */
	if (argc > 5) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 5) {
		printf("too few arguments!\n");
		return 1;
	} else {
		/* If the arguments are not in specific order this loop filters them*/
		for (i = 0; i < argc; i++) {
			if (strncmp(argv[i], "-d", 2) == 0) {
				minusD = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-n", 2) != 0) {
					period = atoi(argv[i + 1]);
				} else {
					NOperiod = 1;
				}
			} else if (strncmp(argv[i], "-s", 2) == 0) {
				minusS = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-n", 2) != 0) {
					shmid = atoi(argv[i + 1]);
				} else {
					NOshmid = 1;
				}
			}
		}
		if (minusD == 0) {
			printf("flag -d is missing.\ntype -d [TIME] \nprogram terminated\n");
			return 1;
		}
		if (minusS == 0) {
			printf("flag -s is missing.\ntype -s [SHMID] \nprogram terminated\n");
			return 1;
		}
		if (NOperiod == 1) {
			printf("time not entered\ntype -d [TIME] \nprogram terminated\n");
			return 1;
		}
		if (NOshmid == 1) {
			printf("shared memory ID not entered\ntype -s [SHMID] \nprogram terminated\n");
			return 1;
		}

	}
	Doorman(period, shmid);
}
