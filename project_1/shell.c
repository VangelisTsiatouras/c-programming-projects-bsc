/*
 * shell.c
 *
 *  Created on: Oct 10, 2016
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "shell.h"

#define BUFFERSIZE 256

void shell(HashTable *hashTable, SkipListNode *skipListHead, char *operationsFile) {
	srand(time(NULL));
	char *buf = malloc(sizeof(char*) * BUFFERSIZE);
	if (buf == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	/* Check if there is an operation file*/
	if (operationsFile != NULL) {
		FILE* opFile;
		opFile = fopen(operationsFile, "r");
		if (opFile == NULL) {
			printf("failure: file not found!\n");
			exit(1);
		}
		/* Read the operation file line by line*/
		while (fgets(buf, BUFFERSIZE, opFile) != NULL) {
			printf("%s", buf);
			if (ShellExecuteCommand(hashTable, skipListHead, buf) == -1) {
				free(buf);
				return;
			}
		}
	}
	/* Now read from stdin*/
	while (fgets(buf, BUFFERSIZE, stdin) != NULL) {
		if (ShellExecuteCommand(hashTable, skipListHead, buf) == -1) {
			free(buf);
			return;
		}
	}
}

/* Return -1 when the input is "e"
 * Return -2 for wrong args
 * exit(1) for system errors
 */
int ShellExecuteCommand(HashTable *hashTable, SkipListNode *skipListHead, char *lineBuffer) {
	char* token;
	char** words = NULL;
	int numberOfSpaces = 0, tokencounter = 0;
	/* Split the entered line to words*/
	token = strtok(lineBuffer, " \t\n");
	while (token != NULL) {
		numberOfSpaces++;
		words = realloc(words, sizeof(char*) * numberOfSpaces);
		if (words == NULL) {
			printf("malloc error %s\n", strerror(errno));
			exit(1);
		}
		words[tokencounter] = token;
		tokencounter++;
		token = strtok(NULL, " \t\n");
	}
	/* Execute given command */
	int charToInt1, charToInt2, charToInt3;
	float charToFloat;
	if (strcmp(words[0], "i") == 0) {
		if (tokencounter < 7) {
			printf("\ntoo few arguments!\n");
			return -2;
		}

      /*if (tokencounter > 7) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
       */
		/* studid*/
		charToInt1 = atoi(words[1]);
		/* gpa*/
		charToFloat = atof(words[4]);
		/* numcourses*/
		charToInt2 = atoi(words[5]);
		/* postcode*/
		charToInt3 = atoi(words[7]);
		if (charToInt1 <= 0) {
			printf("\nwrong studid\nstudid must be greater than 0\n");
			return -2;
		}
		if (charToFloat <= 0) {
			printf("\nwrong gpa\ngpa must be greater than 0\n");
			return -2;
		}
		if (charToInt2 <= 0) {
			printf("\nwrong number of courses\nnumber of courses must be greater than 0\n");
			return -2;
		}
		if (charToInt3 > 99999 || charToInt3 <= 9999) {
			printf("\nwrong postal code\npostal codes are 5 digit numbers\n");
			return -2;
		}
		/* words[2] = lastname
		 * words[3] = firstname
		 * words[6] = deprt
		 */
		Insert(hashTable, skipListHead, charToInt1, words[2], words[3], charToFloat, charToInt2, words[6], charToInt3);
	} else if (strcmp(words[0], "q") == 0) {
		if (tokencounter < 2) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* studid*/
		charToInt1 = atoi(words[1]);
		if (charToInt1 <= 0) {
			printf("\nwrong studid\nstudid must be greater than 0\n");
			return -2;
		}
		Query(skipListHead, charToInt1);
	} else if (strcmp(words[0], "m") == 0) {
		if (tokencounter < 4) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 4) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* studid*/
		charToInt1 = atoi(words[1]);
		/* gpa*/
		charToFloat = atof(words[2]);
		/* numcourses*/
		charToInt2 = atoi(words[3]);
		if (charToInt1 <= 0) {
			printf("\nwrong studid\nstudid must be greater than 0\n");
			return -2;
		}
		if (charToFloat <= 0) {
			printf("\nwrong gpa\ngpa must be greater than 0\n");
			return -2;
		}
		if (charToInt2 <= 0) {
			printf("\nwrong number of courses\nnumber of courses must be greater than 0\n");
			return -2;
		}
		Modify(skipListHead, charToInt1, charToFloat, charToInt2);
	} else if (strcmp(words[0], "d") == 0) {
		if (tokencounter < 2) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* studid*/
		charToInt1 = atoi(words[1]);
		if (charToInt1 <= 0) {
			printf("\nwrong studid\nstudid must be greater than 0\n");
			return -2;
		}
		Delete(hashTable, skipListHead, charToInt1);
	} else if (strcmp(words[0], "ra") == 0) {
		if (tokencounter < 3) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 3) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* studida*/
		charToInt1 = atoi(words[1]);
		/* studidb*/
		charToInt2 = atoi(words[2]);
		if (charToInt1 <= 0) {
			printf("\nwrong studid\nstudid must be greater than 0\n");
			return -2;
		}
		if (charToInt2 <= 0) {
			printf("\nwrong studid\nstudid must be greater than 0\n");
			return -2;
		}
		RAverage(skipListHead, charToInt1, charToInt2);
	} else if (strcmp(words[0], "a") == 0) {
		if (tokencounter < 2) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* postcode*/
		charToInt1 = atoi(words[1]);
		if (charToInt1 > 99999 || charToInt1 <= 9999) {
			printf("\nwrong postal code\npostal codes are 5 digit numbers\n");
			return -2;
		}
		Average(hashTable, charToInt1);
	} else if (strcmp(words[0], "ta") == 0) {
		if (tokencounter < 3) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 3) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* k*/
		charToInt1 = atoi(words[1]);
		/* postcode*/
		charToInt2 = atoi(words[2]);
		if (charToInt1 <= 0) {
			printf("\nk must be greater than 0\n");
			return -2;
		}
		if (charToInt2 > 99999 || charToInt2 <= 9999) {
			printf("\nwrong postal code\npostal codes are 5 digit numbers\n");
			return -2;
		}
		TAverage(hashTable, charToInt2, charToInt1);
	} else if (strcmp(words[0], "b") == 0) {
		if (tokencounter < 2) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* k*/
		charToInt1 = atoi(words[1]);
		if (charToInt1 <= 0) {
			printf("\nk must be greater than 0\n");
			return -2;
		}
		Bottom(skipListHead, charToInt1);
	} else if (strcmp(words[0], "ct") == 0) {
		if (tokencounter < 3) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 3) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		char *deprt = malloc(sizeof(char*) * BUFFERSIZE);
		snprintf(deprt, BUFFERSIZE, "%s", words[2]);
		/* postcode*/
		charToInt1 = atoi(words[1]);
		/* deprt = words[2]*/
		if (charToInt1 > 99999 || charToInt1 <= 9999) {
			printf("\nwrong postal code\npostal codes are 5 digit numbers\n");
			return -2;
		}
		CoursesToTake(hashTable, charToInt1, deprt);
		free(deprt);
	} else if (strcmp(words[0], "f") == 0) {
		if (tokencounter < 2) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* gpa*/
		charToFloat = atof(words[1]);
		if (charToFloat <= 0) {
			printf("\nwrong gpa\ngpa must be greater than 0\n");
			return -2;
		}
		Find(skipListHead, charToFloat);
	} else if (strcmp(words[0], "p") == 0) {
		if (tokencounter < 2) {
			printf("\ntoo few arguments!\n");
			return -2;
		}
		if (tokencounter > 2) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		/* postcode*/
		charToInt1 = atoi(words[1]);
		if (charToInt1 > 99999 || charToInt1 <= 9999) {
			printf("\nwrong postal code\npostal codes are 5 digit numbers\n");
			return -2;
		}
		Percentile(hashTable, charToInt1);
	} else if (strcmp(words[0], "pe") == 0) {
		if (tokencounter > 1) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		PErcentiles(hashTable);
	} else if (strcmp(words[0], "e") == 0) {
		if (tokencounter > 1) {
			printf("\ntoo many arguments!\n");
			return -2;
		}
		DestroySL(skipListHead);
		DestroyHT(hashTable);
		return -1;
	} else if (strcmp(words[0], "\n") == 0) {
		printf("\n");
	} else {
		printf("\nunknown command\n");
		return 0;
	}
	free(token);
	free(words);
	return 0;
}

void Insert(HashTable *hashTable, SkipListNode *skipListHead, int studid, char *lastname, char *firstname, float gpa,
		int numcourses, char *deprt, int postcode) {
	if (SearchRecSL(skipListHead, studid) == 0) {
		printf("\nthis record is already stored!\n");
		return;
	}
	Record *newRec = malloc(sizeof(Record));
	if (newRec == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	newRec->studid = studid;
	newRec->lastname = malloc(sizeof(char*) * BUFFERSIZE - 1);
	if (newRec->lastname == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	snprintf(newRec->lastname, BUFFERSIZE, "%s", lastname);
	newRec->firstname = malloc(sizeof(char*) * BUFFERSIZE);
	if (newRec->firstname == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	snprintf(newRec->firstname, BUFFERSIZE, "%s", firstname);
	newRec->gpa = gpa;
	newRec->numcourses = numcourses;
	newRec->deprt = malloc(sizeof(char*) * BUFFERSIZE);
	if (newRec->deprt == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(1);
	}
	snprintf(newRec->deprt, BUFFERSIZE, "%s", deprt);
	newRec->postcode = postcode;
	InsertRecordSL(skipListHead, newRec);
	InsertRecordHT(hashTable, newRec);
}

void Query(SkipListNode *skipListHead, int studid) {
	Record *rec = SearchReturnRecSL(skipListHead, studid);
	if (rec == NULL) {
		printf("record not found!\n");
	} else {
		printf("%d %s %s %.2f %d %s %d\n", rec->studid, rec->lastname, rec->firstname, rec->gpa, rec->numcourses,
				rec->deprt, rec->postcode);
	}
}

void Modify(SkipListNode *skipListHead, int studid, float gpa, int numcourses) {
	Record *rec = SearchReturnRecSL(skipListHead, studid);
	if (rec == NULL) {
		printf("\nrecord not found!\n");
	} else {
		rec->gpa = gpa;
		rec->numcourses = numcourses;
	}
}

void Delete(HashTable *hashTable, SkipListNode *skipListHead, int studid) {
	Record *rec = SearchReturnRecSL(skipListHead, studid);
	if (rec == NULL) {
		printf("record not found!\n");
	} else {
		printf("%d %s %s %.2f %d %s %d\n", rec->studid, rec->lastname, rec->firstname, rec->gpa, rec->numcourses,
				rec->deprt, rec->postcode);
		DeleteRecordSL(skipListHead, rec->studid);
		DeleteRecordHT(hashTable, rec);
		free(rec->deprt);
		free(rec->lastname);
		free(rec->firstname);
	}
}

void RAverage(SkipListNode *skipListHead, int studida, int studidb) {
	float avrg = RAverageSL(skipListHead, studida, studidb);
	if (avrg > 0) {
		printf("%.2f\n", avrg);
	} else {
		printf("no average\n");
	}
}

void Average(HashTable *hashTable, int postcode) {
	float avrg = AveragePostCodeHT(hashTable, postcode);
	if (avrg > 0) {
		printf("%.2f\n", avrg);
	} else {
		printf("no average\n");
	}
}

void TAverage(HashTable *hashTable, int postcode, int k) {
	if (KTOPAveragePostCodeHT(hashTable, postcode, k) == -1) {
		printf("failure\n");
	}
}

void Bottom(SkipListNode *skipListHead, int k) {
	if (PrintKBottomSL(skipListHead, k) == -1) {
		printf("not found\n");
	}
}

void CoursesToTake(HashTable *hashTable, int postcode, char *deprt) {
	if (CoursesToTakeHT(hashTable, postcode, deprt) != 0) {
		printf("not found\n");
	}
}

void Find(SkipListNode *skipListHead, float gpa) {
	if (FindGPACoursesSL(skipListHead, gpa) != 0) {
		printf("failure\n");
	}
}

void Percentile(HashTable *hashTable, int postcode) {
	float percentile = PercentilePostCodeHT(hashTable, postcode);
	if (percentile != 0) {
		printf("%d %.2f\n", postcode, percentile);
	} else {
		printf("not found\n");
	}
}

void PErcentiles(HashTable *hashTable) {
	if (PercentilesHT(hashTable) != 0) {
		printf("failure\n");
	}
}
