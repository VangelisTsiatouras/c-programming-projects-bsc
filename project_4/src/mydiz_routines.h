/*
 * mydiz_routines.h
 *
 *  Created on: Feb 9, 2017
 *      Author: vangelis
 */

#ifndef MYDIZ_ROUTINES_H_
#define MYDIZ_ROUTINES_H_

#include "dinodes_structure.h"

void Compress(int gzip, char *archiveFile, char **filesArray, int filenumber, char **directoriesArray, int dirnumber);
void Append(int gzip, char *archiveFile, char **filesArray, int filenumber, char **directoriesArray, int dirnumber);
void Extract(char *archiveFile);
void Delete(char *archiveFile, char **filesArray, int filenumber, char **directoriesArray, int dirnumber);
void PrintMetadata(char *archiveFile);
void FileQuery(char *archiveFile, char **listOfFilesDirs, int numberOfFilesDirs);
void PrintTree(char *archiveFile);
void FolderIteration(FILE *diFile, const char *name, DInodeList **list, int *listElements, int parentPosition);
void StructureItetarion(FILE *diFile, DInodeList *list, int listElements, int position, char *path);
void StructureIterationTREE(DInodeList *list, int listElements, int position, int level);
int StructureIterationFIND(char *name, DInodeList *list, int listElements, int position);
int FileCheck(char **listOfFilesDirs, int numberOfFilesDirs);
void PrintDilist(DInodeList *DIlist, int listElements);

#endif /* MYDIZ_ROUTINES_H_ */
