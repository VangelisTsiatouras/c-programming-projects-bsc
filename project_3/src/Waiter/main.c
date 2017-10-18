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

#include "waiter_routines.h"

int main(int argc, char **argv) {
	int i, minusM = 0, minusD = 0, minusS = 0, NOmoneyamount = 0, NOperiod = 0,
			NOshmid = 0;
	int moneyAmount, period, shmid;
	/* Split args */

	if (argc > 7) {
		printf("too many arguments!\n");
		printf("argc %d\n", argc);
		return 1;
	} else if (argc < 7) {
		printf("too few arguments!\n");
		return 1;
	} else {
		/* If the arguments are not in specific order this loop filters them*/
		for (i = 0; i < argc; i++) {
			if (strncmp(argv[i], "-m", 2) == 0) {
				minusM = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-d", 2) != 0
						&& strncmp(argv[i + 1], "-s", 2) != 0) {
					moneyAmount = atoi(argv[i + 1]);
				} else {
					NOmoneyamount = 1;
				}
			} else if (strncmp(argv[i], "-d", 2) == 0) {
				minusD = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-m", 2) != 0
						&& strncmp(argv[i + 1], "-s", 2) != 0) {
					period = atoi(argv[i + 1]);
				} else {
					NOperiod = 1;
				}
			} else if (strncmp(argv[i], "-s", 2) == 0) {
				minusS = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-m", 2) != 0
						&& strncmp(argv[i + 1], "-s", 2) != 0) {
					shmid = atoi(argv[i + 1]);
				} else {
					NOshmid = 1;
				}
			}
		}
		if (minusM == 0) {
			printf(
					"flag -m is missing.\ntype -m [MONEYAMOUNT] \nprogram terminated\n");
			return 1;
		}
		if (minusD == 0) {
			printf(
					"flag -d is missing.\ntype -d [PERIOD] \nprogram terminated\n");
			return 1;
		}
		if (minusS == 0) {
			printf(
					"flag -s is missing.\ntype -s [SHMID] \nprogram terminated\n");
			return 1;
		}
		if (NOmoneyamount == 1) {
			printf(
					"money amount not entered\ntype -m [MONEYAMOUNT] \nprogram terminated\n");
			return 1;
		}
		if (NOperiod == 1) {
			printf(
					"period not entered\ntype -d [PERIOD] \nprogram terminated\n");
			return 1;
		}
		if (NOshmid == 1) {
			printf(
					"shared memory ID not entered\ntype -s [SHMID] \nprogram terminated\n");
			return 1;
		}

	}
	Waiter(period, moneyAmount, shmid);
}
