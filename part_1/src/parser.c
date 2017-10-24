#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "parser.h"

int parser(char *initFile, char *queryFile) {
    FILE *iFile, *qFile;
    iFile = fopen(initFile, "r");
    if(iFile == NULL) {
        printf("fopen error %s\n", strerror(errno));
        exit(-1);
    }
    qFile = fopen(queryFile, "r");
    if(qFile == NULL) {
        printf("fopen error %s\n", strerror(errno));
        exit(-1);
    }

    fclose(iFile);
    fclose(qFile);
    return 0;
}
