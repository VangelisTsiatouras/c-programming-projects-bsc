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

#include "customer_routines.h"

int main(int argc, char **argv) {
	int i, minusN = 0, minusD = 0, minusS = 0, NOnumberOfPeople = 0, NOperiod = 0, NOshmid = 0;
	int people, period, shmid;
	/* Split args */
	if (argc > 7) {
		printf("too many arguments!\n");
		return 1;
	} else if (argc < 7) {
		printf("too few arguments!\n");
		return 1;
	} else {
		/* If the arguments are not in specific order this loop filters them*/
		for (i = 0; i < argc; i++) {
			if (strncmp(argv[i], "-n", 2) == 0) {
				minusN = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-d", 2) != 0 && strncmp(argv[i + 1], "-s", 2) != 0) {
					people = atoi(argv[i + 1]);
				} else {
					NOnumberOfPeople = 1;
				}
			} else if (strncmp(argv[i], "-d", 2) == 0) {
				minusD = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-n", 2) != 0 && strncmp(argv[i + 1], "-s", 2) != 0) {
					period = atoi(argv[i + 1]);
				} else {
					NOperiod = 1;
				}
			} else if (strncmp(argv[i], "-s", 2) == 0) {
				minusS = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-n", 2) != 0 && strncmp(argv[i + 1], "-d", 2) != 0) {
					shmid = atoi(argv[i + 1]);
				} else {
					NOshmid = 1;
				}
			}
		}
		if (minusN == 0) {
			printf("flag -n is missing.\ntype -n [NUMBEROFPEOPLE] \nprogram terminated\n");
			return 1;
		}
		if (minusD == 0) {
			printf("flag -d is missing.\ntype -d [PERIOD] \nprogram terminated\n");
			return 1;
		}
		if (minusS == 0) {
			printf("flag -s is missing.\ntype -s [SHMID] \nprogram terminated\n");
			return 1;
		}
		if (NOnumberOfPeople == 1) {
			printf("number of people not entered\ntype -n [NUMBEROFPEOPLE] \nprogram terminated\n");
			return 1;
		}
		if (NOperiod == 1) {
			printf("period not entered\ntype -d [PERIOD] \nprogram terminated\n");
			return 1;
		}
		if (NOshmid == 1) {
			printf("shared memory ID not entered\ntype -s [SHMID] \nprogram terminated\n");
			return 1;
		}
	}
//	printf("I am a customer group with %d people\n", people);
//	fflush(stdout);
//	sleep(10);
	Customer(people, period, shmid);
}
