/*
 * main.c
 *
 *  Created on: Jan 17, 2017
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mydiz_routines.h"

int main(int argc, char **argv) {
	int i, j, minusC = 0, minusA = 0, minusX = 0, minusJC = 0, minusJA = 0, minusD = 0, minusM = 0, minusQ = 0, minusP =
			0, tooManyFlags = 0;
	char *archiveFile = malloc(sizeof(char) * NAME_SIZE);
	if (archiveFile == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	int numberOfFilesDirs = argc - 3;
	char **listOfFilesDirs = (char**) malloc(sizeof(char*) * numberOfFilesDirs);
	for (i = 0; i < numberOfFilesDirs; i++) {
		listOfFilesDirs[i] = (char*) malloc(sizeof(char) * NAME_SIZE);
		if (listOfFilesDirs[i] == NULL) {
			printf("malloc error %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	int filenumber = 0, dirnumber = 0;
	char **filesArray = NULL;
	char **directoriesArray = NULL;
	/* Split args */
	if (argc < 3) {
		printf("too few arguments!\n");
		return 1;
	} else {
		if (strncmp(argv[1], "-c", 2) == 0) {
			minusC = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-jc", 3) != 0
					&& strncmp(argv[2], "-cj", 3) != 0 && strncmp(argv[2], "-ja", 3) != 0
					&& strncmp(argv[2], "-aj", 3) != 0 && strncmp(argv[2], "-d", 2) != 0
					&& strncmp(argv[2], "-m", 2) != 0 && strncmp(argv[2], "-q", 2) != 0
					&& strncmp(argv[2], "-p", 2) != 0) {
				minusC = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
				i = 0;
				for (j = 3; j < argc; j++) {
					strncpy(listOfFilesDirs[i], argv[j], NAME_SIZE);
					i++;
				}
			} else {
				tooManyFlags = 1;
			}
		} else if (strncmp(argv[1], "-a", 2) == 0) {
			minusA = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-c", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-jc", 3) != 0
					&& strncmp(argv[2], "-cj", 3) != 0 && strncmp(argv[2], "-ja", 3) != 0
					&& strncmp(argv[2], "-aj", 3) != 0 && strncmp(argv[2], "-d", 2) != 0
					&& strncmp(argv[2], "-m", 2) != 0 && strncmp(argv[2], "-q", 2) != 0
					&& strncmp(argv[2], "-p", 2) != 0) {
				minusA = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
				i = 0;
				for (j = 3; j < argc; j++) {
					strncpy(listOfFilesDirs[i], argv[j], NAME_SIZE);
					i++;
				}
			} else {
				tooManyFlags = 1;
			}
		} else if (strncmp(argv[1], "-x", 2) == 0) {
			minusX = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-jc", 3) != 0
					&& strncmp(argv[2], "-cj", 3) != 0 && strncmp(argv[2], "-ja", 3) != 0
					&& strncmp(argv[2], "-aj", 3) != 0 && strncmp(argv[2], "-d", 2) != 0
					&& strncmp(argv[2], "-m", 2) != 0 && strncmp(argv[2], "-q", 2) != 0
					&& strncmp(argv[2], "-p", 2) != 0) {
				minusX = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
			} else {
				tooManyFlags = 1;
			}
		} else if ((strncmp(argv[1], "-jc", 4) == 0) || (strncmp(argv[1], "-cj", 4) == 0)) {
			minusJC = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-ja", 3) != 0 && strncmp(argv[2], "-aj", 3) != 0
					&& strncmp(argv[2], "-d", 2) != 0 && strncmp(argv[2], "-m", 2) != 0
					&& strncmp(argv[2], "-q", 2) != 0 && strncmp(argv[2], "-p", 2) != 0) {
				minusJC = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
				i = 0;
				for (j = 3; j < argc; j++) {
					strncpy(listOfFilesDirs[i], argv[j], NAME_SIZE);
					i++;
				}
			} else {
				tooManyFlags = 1;
			}
		} else if ((strncmp(argv[1], "-ja", 4) == 0) || (strncmp(argv[1], "-aj", 4) == 0)) {
			minusJA = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-jc", 3) != 0 && strncmp(argv[2], "-cj", 3) != 0
					&& strncmp(argv[2], "-d", 2) != 0 && strncmp(argv[2], "-m", 2) != 0
					&& strncmp(argv[2], "-q", 2) != 0 && strncmp(argv[2], "-p", 2) != 0) {
				minusJA = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
				i = 0;
				for (j = 3; j < argc; j++) {
					strncpy(listOfFilesDirs[i], argv[j], NAME_SIZE);
					i++;
				}
			} else {
				tooManyFlags = 1;
			}
		} else if (strncmp(argv[1], "-d", 3) == 0) {
			minusD = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-jc", 3) != 0 && strncmp(argv[2], "-cj", 3) != 0
					&& strncmp(argv[2], "-ja", 3) != 0 && strncmp(argv[2], "-aj", 3) != 0
					&& strncmp(argv[2], "-m", 2) != 0 && strncmp(argv[2], "-q", 2) != 0
					&& strncmp(argv[2], "-p", 2) != 0) {
				minusD = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
				i = 0;
				for (j = 3; j < argc; j++) {
					strncpy(listOfFilesDirs[i], argv[j], NAME_SIZE);
					i++;
				}
			} else {
				tooManyFlags = 1;
			}
		} else if (strncmp(argv[1], "-m", 2) == 0) {
			minusM = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-jc", 3) != 0 && strncmp(argv[2], "-cj", 3) != 0
					&& strncmp(argv[2], "-ja", 3) != 0 && strncmp(argv[2], "-aj", 3) != 0
					&& strncmp(argv[2], "-q", 2) != 0 && strncmp(argv[2], "-p", 2) != 0
					&& strncmp(argv[2], "-d", 2) != 0) {
				minusM = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
			} else {
				tooManyFlags = 1;
			}
		} else if (strncmp(argv[1], "-q", 2) == 0) {
			minusQ = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-jc", 3) != 0 && strncmp(argv[2], "-cj", 3) != 0
					&& strncmp(argv[2], "-ja", 3) != 0 && strncmp(argv[2], "-aj", 3) != 0
					&& strncmp(argv[2], "-m", 2) != 0 && strncmp(argv[2], "-p", 2) != 0
					&& strncmp(argv[2], "-d", 2) != 0) {
				minusQ = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
				i = 0;
				for (j = 3; j < argc; j++) {
					strncpy(listOfFilesDirs[i], argv[j], NAME_SIZE);
					i++;
				}
			} else {
				tooManyFlags = 1;
			}
		} else if (strncmp(argv[1], "-p", 2) == 0) {
			minusP = 1;
			if (argv[2] != NULL && strncmp(argv[2], "-a", 2) != 0 && strncmp(argv[2], "-x", 2) != 0
					&& strncmp(argv[2], "-j", 2) != 0 && strncmp(argv[2], "-c", 2) != 0
					&& strncmp(argv[2], "-jc", 3) != 0 && strncmp(argv[2], "-cj", 3) != 0
					&& strncmp(argv[2], "-ja", 3) != 0 && strncmp(argv[2], "-aj", 3) != 0
					&& strncmp(argv[2], "-q", 2) != 0 && strncmp(argv[2], "-m", 2) != 0
					&& strncmp(argv[2], "-d", 2) != 0) {
				minusP = 1;
				strncpy(archiveFile, argv[2], NAME_SIZE);
			} else {
				tooManyFlags = 1;
			}
		}
		if (tooManyFlags == 1) {
			printf(
					"Too many flags entered.\nType ./mydiz {-c|-a|-x|-m|-d|-p|-j|-jc|-ja} <archive-file> <list-of-files/dirs>\nOnly one flag is allowed per execution\nProgram terminated\n");
			return 1;
		}
		/* Sanitize files/folders*/
		if (minusC == 1 || minusA == 1 || minusJC == 1 || minusJA == 1 || minusD == 1) {
			/* Check files if exist*/
			if (FileCheck(listOfFilesDirs, numberOfFilesDirs) == -1) {
				return -1;
			}
			/* Separate files and directories*/
			struct stat path_stat;
			for (i = 0; i < numberOfFilesDirs; i++) {
				stat(listOfFilesDirs[i], &path_stat);
				/* If it is regular file*/
				if (S_ISREG(path_stat.st_mode)) {
					filesArray = (char**) realloc(filesArray, sizeof(char*) * (filenumber + 1));
					char tempfile[NAME_SIZE];
					strncpy(tempfile, listOfFilesDirs[i], NAME_SIZE);
					filesArray[filenumber] = tempfile;
					filenumber++;
				}
				/* If it is directory*/
				if (S_ISDIR(path_stat.st_mode)) {
					directoriesArray = (char**) realloc(directoriesArray, sizeof(char*) * (dirnumber + 1));
					char tempdir[NAME_SIZE];
					strncpy(tempdir, listOfFilesDirs[i], NAME_SIZE);
					directoriesArray[dirnumber] = tempdir;
					dirnumber++;
				}
			}
		}
		/* Check if .di file exists*/
		if (minusX == 1 || minusM == 1 || minusP == 1) {
			if (access(archiveFile, F_OK) == -1) {
				printf("'%s' does not exists\n", archiveFile);
			}
		}
		if (minusC == 1) {
			Compress(0, archiveFile, filesArray, filenumber, directoriesArray, dirnumber);
		} else if (minusA == 1) {
			//WIP
		} else if (minusX == 1) {
			Extract(archiveFile);
		} else if (minusJC == 1) {
			//WIP
		} else if (minusJA == 1) {
			//WIP
		} else if (minusD == 1) {
			//WIP
		} else if (minusM == 1) {
			PrintMetadata(archiveFile);
		} else if (minusQ == 1) {
			FileQuery(archiveFile, listOfFilesDirs, numberOfFilesDirs);
		} else if (minusP == 1) {
			PrintTree(archiveFile);
		}
	}
	free(archiveFile);
	for (i = 0; i < numberOfFilesDirs; i++) {
		free(listOfFilesDirs[i]);
	}
	free(listOfFilesDirs);
	for (i = 0; i < filenumber; i++) {
		free(filesArray[i]);
	}
	free(filesArray);
	for (i = 0; i < filenumber; i++) {
		free(directoriesArray[i]);
	}
	free(directoriesArray);
}
