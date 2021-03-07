/*
 * mydiz_routines.c
 *
 *  Created on: Feb 9, 2017
 *      Author: vangelis
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mydiz_routines.h"
#include "di_header.h"

char *giberrish;
int offset = 0;
int blockCounter = 0;

void Compress(int gzip, char *archiveFile, char **filesArray, int filenumber, char **directoriesArray, int dirnumber) {
	FILE *diFile = fopen(archiveFile, "wb+");
	struct stat buf;
	DInodeList *DIlist = NULL;
	DInodeList *temp;
	Header *header = malloc(sizeof(Header));
	if (header == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	strncpy(header->identifier, "I AM A DI FILE", 16);
	header->pointerToMeta = -1;
	header->totalSize = -1;
	header->blocksOfMeta = -1;
	int listElements = 0;
	int currentDirPosition;
	int i, j;
	giberrish = calloc( BLOCK_SIZE, sizeof(char));
	if (giberrish == NULL) {
		printf("calloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Write one block to the file for the use of header*/
	fwrite(header, 1, sizeof(Header), diFile);
	fwrite(giberrish, 1, BLOCK_SIZE - sizeof(Header), diFile);
	offset += BLOCK_SIZE;
	blockCounter++;
	/* The parent folder which contains the files/dirs that will be archived*/
	DIlist = malloc(sizeof(DInodeList) * (listElements + 1));
	if (DIlist == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Initialize first node*/
	for (j = 0; j < DINODES_PER_DIRECTORY; j++) {
		DIlist[0].dinode[j].positionInList = -1;
	}
	strncpy(DIlist[0].name, archiveFile, NAME_SIZE);
	DIlist[0].flagDirectory = 1;
	DIlist[0].permissions = 0777;
	strncpy(DIlist[0].dinode[0].name, ".", NAME_SIZE);
	DIlist[0].dinode[0].positionInList = 0;
	strncpy(DIlist[0].dinode[1].name, "..\0", NAME_SIZE);
	DIlist[0].dinode[1].positionInList = 0;
	listElements++;
	/* Add given directories to the .di file*/
	if (dirnumber != 0) {
		for (i = 0; i < dirnumber; i++) {
			temp = realloc(DIlist, sizeof(DInodeList) * (listElements + 1));
			if (temp == NULL) {
				printf("realloc error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				DIlist = temp;
			}
			strncpy(DIlist[listElements].name, directoriesArray[i], NAME_SIZE);
			DIlist[listElements].flagDirectory = 1;
			stat(directoriesArray[i], &buf);
			DIlist[listElements].permissions = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			DIlist[listElements].size = buf.st_size;
			DIlist[listElements].atime = buf.st_atime;
			DIlist[listElements].ctime = buf.st_ctime;
			DIlist[listElements].mtime = buf.st_mtime;
			DIlist[listElements].ptrToFile = -1;
			for (j = 0; j < DINODES_PER_DIRECTORY; j++) {
				strncpy(DIlist[listElements].dinode[j].name, "", NAME_SIZE);
				DIlist[listElements].dinode[j].positionInList = -1;
			}
			strncpy(DIlist[listElements].dinode[0].name, ".", NAME_SIZE);
			DIlist[listElements].dinode[0].positionInList = listElements;
			strncpy(DIlist[listElements].dinode[1].name, "..", NAME_SIZE);
			DIlist[listElements].dinode[1].positionInList = 0;
			currentDirPosition = listElements;
			/* Write to parent dinode*/
			j = 0;
			while (DIlist[0].dinode[j].positionInList != -1) {
				j++;
			}
			strncpy(DIlist[0].dinode[j].name, directoriesArray[i], NAME_SIZE);
			DIlist[0].dinode[j].positionInList = listElements;
			listElements++;
			/* Iterate the current folder*/
			FolderIteration(diFile, directoriesArray[i], &DIlist, &listElements, currentDirPosition);
		}

	}
	/* Add given files to the .di file*/
	if (filenumber != 0) {
		for (i = 0; i < filenumber; i++) {
			temp = realloc(DIlist, sizeof(DInodeList) * (listElements + 1));
			if (temp == NULL) {
				printf("realloc error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				DIlist = temp;
			}
			strncpy(DIlist[listElements].name, filesArray[i], NAME_SIZE);
			DIlist[listElements].flagDirectory = 0;
			stat(filesArray[i], &buf);
			DIlist[listElements].permissions = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			DIlist[listElements].size = buf.st_size;
			DIlist[listElements].atime = buf.st_atime;
			DIlist[listElements].ctime = buf.st_ctime;
			DIlist[listElements].mtime = buf.st_mtime;
			DIlist[listElements].ptrToFile = offset / BLOCK_SIZE;
			for (j = 0; j < DINODES_PER_DIRECTORY; j++) {
				strncpy(DIlist[listElements].dinode[j].name, "", NAME_SIZE);
				DIlist[listElements].dinode[j].positionInList = -1;
			}
			/* Write to parent dinode*/
			j = 0;
			while (DIlist[0].dinode[j].positionInList != -1) {
				j++;
			}
			strncpy(DIlist[0].dinode[j].name, filesArray[i], NAME_SIZE);
			DIlist[0].dinode[j].positionInList = listElements;
			/* Store file into .di file*/
			/* Open file*/
			FILE *fileToStore = fopen(filesArray[i], "rb");
			char buffer[BLOCK_SIZE];
			int blocksToWrite = 0;
			int semiFullBlock = 0;
			blocksToWrite = (DIlist[listElements].size) / BLOCK_SIZE;
			if (((DIlist[listElements].size) % BLOCK_SIZE) != 0) {
				blocksToWrite++;
				semiFullBlock = 1;
			}
			for (j = 0; j < blocksToWrite; j++) {
				fseek(diFile, offset, SEEK_SET);
				if (j == (blocksToWrite - 1) && semiFullBlock == 1) {
					int giber = (blocksToWrite * BLOCK_SIZE) - (DIlist[listElements].size);
					fread(buffer, BLOCK_SIZE - giber, 1, fileToStore);
					fwrite(buffer, 1, BLOCK_SIZE - giber, diFile);
					fwrite(giberrish, 1, giber, diFile);
					offset += BLOCK_SIZE;
				} else {
					fread(buffer, BLOCK_SIZE, 1, fileToStore);
					fwrite(buffer, 1, BLOCK_SIZE, diFile);
					offset += BLOCK_SIZE;
				}
				blockCounter++;
				fflush(diFile);
			}
			fclose(fileToStore);
			listElements++;
		}
	}
	/* Write metadata into .di file*/
	for (i = 0; i < listElements; i++) {
		fwrite(&DIlist[i], 1, sizeof(DInodeList), diFile);
		fwrite(giberrish, 1, BLOCK_SIZE - sizeof(DInodeList), diFile);
		offset += BLOCK_SIZE;
	}
	/* Update header section*/
	rewind(diFile);
	stat(archiveFile, &buf);
	strncpy(header->identifier, "I AM A DI FILE", 16);
	header->blocksOfMeta = listElements;
	header->pointerToMeta = blockCounter;
	header->totalSize = buf.st_size;
	fwrite(header, 1, sizeof(Header), diFile);
	fwrite(giberrish, 1, BLOCK_SIZE - sizeof(Header), diFile);
	/* Free allocated space*/
	free(header);
	free(giberrish);
	free(DIlist);
	fclose(diFile);
	printf("compress completed\n");
}

void Append(int gzip, char *archiveFile, char **filesArray, int filenumber, char **directoriesArray, int dirnumber) {
	//WIP
	return;
}

void Extract(char *archiveFile) {
	int i;
	FILE *diFile = fopen(archiveFile, "rb");
	/* Read header section*/
	Header *header = malloc(sizeof(Header));
	if (header == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fread(header, sizeof(Header), 1, diFile);
	/* Check if this file is not di*/
	if (strncmp(header->identifier, "I AM A DI FILE", 16) != 0) {
		printf("this file is not di\n");
		exit(1);
	}
	/* Store metadata locally*/
	int listElements = header->blocksOfMeta;
	DInodeList *DIlist = malloc(sizeof(DInodeList) * listElements);
	fseek(diFile, (header->pointerToMeta) * BLOCK_SIZE, SEEK_SET);
	for (i = 0; i < header->blocksOfMeta; i++) {
		fread(&DIlist[i], sizeof(DInodeList), 1, diFile);
		fseek(diFile, BLOCK_SIZE - sizeof(DInodeList), SEEK_CUR);
	}
	char rootDir[256];
	snprintf(rootDir, 256, "%s_extract", archiveFile);
	/* Call recursive function to extract files/dirs*/
	StructureItetarion(diFile, DIlist, listElements, 0, rootDir);
	/* Free allocated space*/
	free(header);
	free(DIlist);
	fclose(diFile);
	printf("extract completed\n");
}

void Delete(char *archiveFile, char **filesArray, int filenumber, char **directoriesArray, int dirnumber) {
	//WIP
	return;
}

void PrintMetadata(char *archiveFile) {
	int i, j, z;
	char *modes[] = { "---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx" };
	char type, perms[10];
	FILE *diFile = fopen(archiveFile, "rb");
	/* Read header section*/
	Header *header = malloc(sizeof(Header));
	if (header == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fread(header, sizeof(Header), 1, diFile);
	/* Check if this file is not di*/
	if (strncmp(header->identifier, "I AM A DI FILE", 16) != 0) {
		printf("this file is not di\n");
		exit(1);
	}
	/* Store metadata locally*/
	int listElements = header->blocksOfMeta;
	DInodeList *DIlist = malloc(sizeof(DInodeList) * listElements);
	fseek(diFile, (header->pointerToMeta) * BLOCK_SIZE, SEEK_SET);
	for (i = 0; i < header->blocksOfMeta; i++) {
		fread(&DIlist[i], sizeof(DInodeList), 1, diFile);
		fseek(diFile, BLOCK_SIZE - sizeof(DInodeList), SEEK_CUR);
	}
	/* Iterate the DiList and print metadata*/
	for (i = 0; i < listElements; i++) {
		if (DIlist[i].flagDirectory == 1) {
			type = '-';
		} else {
			type = 'd';
		}
		*perms = '\0';
		for (j = 2; j >= 0; j--) {
			z = (DIlist[i].permissions >> (j * 3)) & 07;
			strcat(perms, modes[z]);
		}
		printf("\n%s\n%c%s %d\n%s%s%s\n", DIlist[i].name, type, perms, DIlist[i].size,
				(char *) (ctime(&DIlist[i].atime)), (char *) (ctime(&DIlist[i].mtime)),
				(char *) (ctime(&DIlist[i].ctime)));
	}
	/* Free allocated space*/
	free(header);
	free(DIlist);
	fclose(diFile);
}

void FileQuery(char *archiveFile, char **listOfFilesDirs, int numberOfFilesDirs) {
	int i;
	FILE *diFile = fopen(archiveFile, "rb");
	/* Read header section*/
	Header *header = malloc(sizeof(Header));
	if (header == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fread(header, sizeof(Header), 1, diFile);
	/* Check if this file is not di*/
	if (strncmp(header->identifier, "I AM A DI FILE", 16) != 0) {
		printf("this file is not di\n");
		exit(1);
	}
	/* Store metadata locally*/
	int listElements = header->blocksOfMeta;
	DInodeList *DIlist = malloc(sizeof(DInodeList) * listElements);
	fseek(diFile, (header->pointerToMeta) * BLOCK_SIZE, SEEK_SET);
	for (i = 0; i < header->blocksOfMeta; i++) {
		fread(&DIlist[i], sizeof(DInodeList), 1, diFile);
		fseek(diFile, BLOCK_SIZE - sizeof(DInodeList), SEEK_CUR);
	}
	/* Call recursive function to find the given files/dirs*/
	for (i = 0; i < numberOfFilesDirs; i++) {
		if (StructureIterationFIND(listOfFilesDirs[i], DIlist, listElements, 0) == 1) {
			printf("'%s' exists\n", listOfFilesDirs[i]);
		} else {
			printf("'%s' not exist\n", listOfFilesDirs[i]);
		}
	}
	free(header);
	free(DIlist);
	fclose(diFile);
}

void PrintTree(char *archiveFile) {
	int i;
	FILE *diFile = fopen(archiveFile, "rb");
	/* Read header section*/
	Header *header = malloc(sizeof(Header));
	if (header == NULL) {
		printf("malloc error %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fread(header, sizeof(Header), 1, diFile);
	/* Check if this file is not di*/
	if (strncmp(header->identifier, "I AM A DI FILE", 16) != 0) {
		printf("this file is not di\n");
		exit(1);
	}
	/* Store metadata locally*/
	int listElements = header->blocksOfMeta;
	DInodeList *DIlist = malloc(sizeof(DInodeList) * listElements);
	fseek(diFile, (header->pointerToMeta) * BLOCK_SIZE, SEEK_SET);
	for (i = 0; i < header->blocksOfMeta; i++) {
		fread(&DIlist[i], sizeof(DInodeList), 1, diFile);
		fseek(diFile, BLOCK_SIZE - sizeof(DInodeList), SEEK_CUR);
	}
	/* Call recursive function to print "tree"*/
	StructureIterationTREE(DIlist, listElements, 0, 0);
	/* Free allocated space*/
	free(header);
	free(DIlist);
	fclose(diFile);
}

void FolderIteration(FILE *diFile, const char *name, DInodeList **list, int *listElements, int parentPosition) {
	DIR *dir;
	struct stat buf;
	DInodeList *temp1;
	struct dirent *entry;
	int i, j;
	int currentDirPosition;
	char path[1024];
	if (!(dir = opendir(name)))
		return;
	if (!(entry = readdir(dir)))
		return;
	do {
		int len = snprintf(path, sizeof(path) - 1, "%s/%s", name, entry->d_name);
		path[len] = 0;
		/*If it is directory */
		if (entry->d_type == DT_DIR) {
			currentDirPosition = *listElements;
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			int newsize = (int) (*listElements) + 1;
			temp1 = (DInodeList*) realloc(*list, sizeof(DInodeList) * newsize);
			if (temp1 == NULL) {
				printf("realloc error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				*list = temp1;
			}
			strncpy((*list)[(*listElements)].name, entry->d_name, NAME_SIZE);
			(*list)[(*listElements)].flagDirectory = 1;
			stat(path, &buf);
			(*list)[(*listElements)].permissions = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			(*list)[(*listElements)].size = buf.st_size;
			(*list)[(*listElements)].atime = buf.st_atime;
			(*list)[(*listElements)].ctime = buf.st_ctime;
			(*list)[(*listElements)].mtime = buf.st_mtime;
			(*list)[(*listElements)].ptrToFile = -1;
			for (i = 0; i < DINODES_PER_DIRECTORY; i++) {
				strncpy((*list)[(*listElements)].dinode[i].name, "", NAME_SIZE);
				(*list)[(*listElements)].dinode[i].positionInList = -1;
			}
			strncpy((*list)[(*listElements)].dinode[0].name, ".", NAME_SIZE);
			(*list)[(*listElements)].dinode[0].positionInList = (int) (*listElements);
			strncpy((*list)[(*listElements)].dinode[1].name, "..", NAME_SIZE);
			(*list)[(*listElements)].dinode[1].positionInList = parentPosition;
			/* Write to parent dinode*/
			j = 0;
			while ((*list)[parentPosition].dinode[j].positionInList != -1) {
				j++;
			}
			strncpy((*list)[parentPosition].dinode[j].name, entry->d_name, NAME_SIZE);
			(*list)[parentPosition].dinode[j].positionInList = *listElements;
			(*listElements)++;
			/* Recursion*/
			FolderIteration(diFile, path, list, listElements, currentDirPosition);
		}
		/* If it is not directory*/
		else {
			int newsize = (int) (*listElements) + 1;
			temp1 = (DInodeList*) realloc(*list, sizeof(DInodeList) * newsize);
			if (temp1 == NULL) {
				printf("realloc error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				*list = temp1;
			}
			strncpy((*list)[(*listElements)].name, entry->d_name, NAME_SIZE);
			(*list)[(*listElements)].flagDirectory = 0;
			stat(path, &buf);
			(*list)[(*listElements)].permissions = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			(*list)[(*listElements)].size = buf.st_size;
			(*list)[(*listElements)].atime = buf.st_atime;
			(*list)[(*listElements)].ctime = buf.st_ctime;
			(*list)[(*listElements)].mtime = buf.st_mtime;
			(*list)[(*listElements)].ptrToFile = offset / BLOCK_SIZE;
			for (i = 0; i < DINODES_PER_DIRECTORY; i++) {
				strncpy((*list)[(*listElements)].dinode[i].name, "", NAME_SIZE);
				(*list)[(*listElements)].dinode[i].positionInList = -1;
			}
			/* Write to parent dinode*/
			j = 0;
			while ((*list)[parentPosition].dinode[j].positionInList != -1) {
				j++;
			}
			strncpy((*list)[parentPosition].dinode[j].name, entry->d_name, NAME_SIZE);
			(*list)[parentPosition].dinode[j].positionInList = (int) *listElements;
			/* Store file into .di file*/
			/* Open file*/
			FILE *fileToStore = fopen(path, "rb");
			char buffer[BLOCK_SIZE];
			int blocksToWrite = 0;
			int semiFullBlock = 0;
			blocksToWrite = ((*list)[(*listElements)].size) / BLOCK_SIZE;
			if ((((*list)[(*listElements)].size) % BLOCK_SIZE) != 0) {
				blocksToWrite++;
				semiFullBlock = 1;
			}
			for (i = 0; i < blocksToWrite; i++) {
				fseek(diFile, offset, SEEK_SET);
				if (i == (blocksToWrite - 1) && semiFullBlock == 1) {
					int giber = (blocksToWrite * BLOCK_SIZE) - ((*list)[(*listElements)].size);
					fread(buffer, BLOCK_SIZE - giber, 1, fileToStore);
					fwrite(buffer, 1, BLOCK_SIZE - giber, diFile);
					fwrite(giberrish, 1, giber, diFile);
					offset += BLOCK_SIZE;
				} else {
					fread(buffer, BLOCK_SIZE, 1, fileToStore);
					fwrite(buffer, 1, BLOCK_SIZE, diFile);
					offset += BLOCK_SIZE;
				}
				blockCounter++;
				fflush(diFile);

			}
			(*listElements)++;
			fclose(fileToStore);
		}
	} while ((entry = readdir(dir)));
	closedir(dir);
}

void StructureItetarion(FILE *diFile, DInodeList *list, int listElements, int position, char *path) {
	int i;
	char buf[256];
	if (list[position].flagDirectory == 1) {
		/* Create directory*/
		mode_t mask = umask(0);
		if (mkdir(path, list[position].permissions) == -1) {
			printf("mkdir error %s\n", strerror(errno));
		}
		umask(mask);
		for (i = 0; i < DINODES_PER_DIRECTORY; i++) {
			if (strcmp(list[position].dinode[i].name, ".") != 0 && strcmp(list[position].dinode[i].name, "..") != 0) {
				if (list[position].dinode[i].positionInList != -1) {
					snprintf(buf, 256, "%s/%s", path, list[position].dinode[i].name);
					/* Recursion*/
					StructureItetarion(diFile, list, listElements, list[position].dinode[i].positionInList, buf);
				} else {
					break;
				}
			} else {
				continue;
			}
		}
	} else {
		/* Extract file*/
		struct stat st;
		struct utimbuf new_times;
		char buffer[BLOCK_SIZE];
		snprintf(buf, 256, "%s/%s", path, list[position].name);
		int extracted = open(path, O_CREAT | O_RDWR, list[position].permissions);
		fseek(diFile, (list[position].ptrToFile) * BLOCK_SIZE, SEEK_SET);
		int remainingBytes = list[position].size;
		while (remainingBytes >= BLOCK_SIZE) {
			fread(buffer, BLOCK_SIZE, 1, diFile);
			write(extracted, buffer, BLOCK_SIZE);
			remainingBytes -= BLOCK_SIZE;
		}
		if (remainingBytes > 0) {
			fread(buffer, remainingBytes, 1, diFile);
			write(extracted, buffer, remainingBytes);
			remainingBytes -= remainingBytes;
		}
		close(extracted);
		stat(path, &st);
		new_times.actime = list[position].atime;
		new_times.modtime = list[position].mtime;
		utime(path, &new_times);
	}
}

void StructureIterationTREE(DInodeList *list, int listElements, int position, int level) {
	int i;
	if (list[position].flagDirectory == 1) {
		printf("%.*s[%s]\n", level * 2, "..................................................", list[position].name);
		for (i = 0; i < DINODES_PER_DIRECTORY; i++) {
			if (strcmp(list[position].dinode[i].name, ".") != 0 && strcmp(list[position].dinode[i].name, "..") != 0) {
				if (list[position].dinode[i].positionInList != -1) {
					/* Recursion*/
					StructureIterationTREE(list, listElements, list[position].dinode[i].positionInList, level + 1);
				} else {
					break;
				}
			} else {
				continue;
			}
		}
	} else {
		printf("%.*s[%s]\n", level * 2, "..................................................", list[position].name);
	}
}

int StructureIterationFIND(char *name, DInodeList *list, int listElements, int position) {
	int i;
	if (list[position].flagDirectory == 1) {
		/* If directory found return 1, otherwise 0*/
		if (strncmp(list[position].name, name, NAME_SIZE) == 0) {
			return 1;
		}
		for (i = 0; i < DINODES_PER_DIRECTORY; i++) {
			if (strcmp(list[position].dinode[i].name, ".") != 0 && strcmp(list[position].dinode[i].name, "..") != 0) {
				if (list[position].dinode[i].positionInList != -1) {
					/* Recursion*/
					if (StructureIterationFIND(name, list, listElements, list[position].dinode[i].positionInList)
							== 1) {
						/* If file/dir founded in "lower level" pass it to the parent*/
						return 1;
					}
				} else {
					return 0;
				}
			} else {
				continue;
			}
		}
	} else {
		/* If file found return 1, otherwise 0*/
		if (strncmp(list[position].name, name, NAME_SIZE) == 0) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}

int FileCheck(char **listOfFilesDirs, int numberOfFilesDirs) {
	int i, flag = 0;
	for (i = 0; i < numberOfFilesDirs; i++) {
		if (access(listOfFilesDirs[i], F_OK) == -1) {
			printf("'%s' does not exist\n", listOfFilesDirs[i]);
			flag = 1;
		}
	}
	if (flag == 1) {
		return -1;
	}
	return 0;
}

void PrintDilist(DInodeList *DIlist, int listElements) {
	int i, j;
	for (i = 0; i < listElements; i++) {
		printf(
				"Node Number: %d\nName: %s Perms: %o\nSize: %d\nPointer to File %d\nAccess time %sModify time %sCreate time %s",
				i, DIlist[i].name, DIlist[i].permissions, DIlist[i].size, DIlist[i].ptrToFile,
				(char *) (ctime(&DIlist[i].atime)), (char *) (ctime(&DIlist[i].mtime)),
				(char *) (ctime(&DIlist[i].ctime)));
		for (j = 0; j < DINODES_PER_DIRECTORY; j++) {
			if (DIlist[i].dinode[j].positionInList != -1) {
				printf("	dinode %d, name %s, position in list %d\n", j, DIlist[i].dinode[j].name,
						DIlist[i].dinode[j].positionInList);
			}
		}
	}
}
