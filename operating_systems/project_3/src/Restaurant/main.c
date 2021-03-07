/*
 * main.c
 *
 *  Created on: Dec 3, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "restaurant_routines.h"

#define BUFFERSIZE 256

int main(int argc, char **argv) {
	int i, minusN = 0, minusD = 0, minusL = 0, NOcustomers = 0, NOperiod = 0, NOconfigfile = 0;
	int customers, period;
	char *configFile = malloc(sizeof(char) * BUFFERSIZE);
	if (configFile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Init srand*/
	srand(time(NULL));
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
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-d", 2) != 0 && strncmp(argv[i + 1], "-l", 2) != 0) {
					customers = atoi(argv[i + 1]);
				} else {
					NOcustomers = 1;
				}
			} else if (strncmp(argv[i], "-d", 2) == 0) {
				minusD = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-n", 2) != 0 && strncmp(argv[i + 1], "-l", 2) != 0) {
					period = atoi(argv[i + 1]);
				} else {
					NOperiod = 1;
				}
			} else if (strncmp(argv[i], "-l", 2) == 0) {
				minusL = 1;
				if (argv[i + 1] != NULL && strncmp(argv[i + 1], "-n", 2) != 0 && strncmp(argv[i + 1], "-d", 2) != 0) {
					strncpy(configFile, argv[i + 1], BUFFERSIZE);
				} else {
					NOconfigfile = 1;
				}
			}
		}
		if (minusN == 0) {
			printf("flag -n is missing.\ntype -n [CUSTOMERS] \nprogram terminated\n");
			return 1;
		}
		if (minusD == 0) {
			printf("flag -d is missing.\ntype -d [TIME] \nprogram terminated\n");
			return 1;
		}
		if (minusL == 0) {
			printf("flag -s is missing.\ntype -l [CONFIGFILE] \nprogram terminated\n");
			return 1;
		}
		if (NOcustomers == 1) {
			printf("number of customers not entered\ntype -n [CUSTOMERS] \nprogram terminated\n");
			return 1;
		}
		if (NOperiod == 1) {
			printf("time not entered\ntype -d [TIME] \nprogram terminated\n");
			return 1;
		}
		if (NOconfigfile == 1) {
			printf("configfile not entered\ntype -l [CONFIGFILE] \nprogram terminated\n");
			return 1;
		}

	}
	/* Restaurant variables*/
	int numberOfWaiters, numberOfTables, maxSizeOfTable, customerPeriod, doormanPeriod, waiterPeriod,
			moneyamount;
	/* Open configfile*/
	FILE* cfgFile;
	cfgFile = fopen(configFile, "r");
	if (cfgFile == NULL) {
		printf("failure: file not found!\n");
		exit(EXIT_FAILURE);
	}
	char buf[BUFFERSIZE];
	i = 0;
	//TODO me mia fscanf bgainei to diavasma
	//alla 8a einai ephrepes se la8h me space kai enter
	/* Read the configfile line by line*/
	while (fgets(buf, BUFFERSIZE, cfgFile) != NULL) {
		/* First line contains the number of waiters*/
		if (i == 0) {
			sscanf(buf, "number of waiters: %d", &numberOfWaiters);
		}
		/* Second line contains the number of tables*/
		else if (i == 1) {
			sscanf(buf, "number of tables: %d", &numberOfTables);
		}
		/* Third line contains the maximum size of tables*/
		else if (i == 2) {
			sscanf(buf, "max size of table: %d", &maxSizeOfTable);
		}
		/* Fifth line contains the customer period*/
		else if (i == 3) {
			sscanf(buf, "customer period: %d", &customerPeriod);
		}
		/* Sixth line contains the doorman period*/
		else if (i == 4) {
			sscanf(buf, "doorman period: %d", &doormanPeriod);
		}
		/* Seventh line contains the waiter period*/
		else if (i == 5) {
			sscanf(buf, "waiter period: %d", &waiterPeriod);
		}
		/* Eighth line contains the max money amount*/
		else if (i == 6) {
			sscanf(buf, "moneyamount: %d", &moneyamount);
		}
		i++;
	}
	RestaurantRoutine(customers, period, numberOfWaiters, numberOfTables, maxSizeOfTable, customerPeriod,
			doormanPeriod, waiterPeriod, moneyamount);
	printf("Restaurant is closing...\n");
	fclose(cfgFile);
	free(configFile);
}
