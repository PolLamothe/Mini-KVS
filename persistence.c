#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hashage.h"
#include "persistence.h"

FILE* openHashMapFile(char* filename){
    FILE* fp = fopen(filename, "rb");
    if(!fp){
        return NULL;
    }
    return fp;
}

void createHashMapFile(char* filename,HashMap* hashmap,Error** error){
    char functionName[] = "hashage.createHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }

    FILE* fp = openHashMapFile(filename);
    if( fp != NULL){
        fclose(fp);
        createError(error,functionName,"File already exist",NULL,NULL);
        return;
    }

    fp = fopen(filename, "wb");
    int entrySize = (int)hashmap->entrySize;
    fwrite(&entrySize,sizeof(int),1,fp);
    fwrite(&hashmap->dataSize,sizeof(int),1,fp);
    fclose(fp);
    return;
}

int* importHashMapFromFile(char* filename,Error** error){
    char functionName[] = "hashage.importHashMapFromFile";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return NULL;
    }
    FILE* fp = openHashMapFile(filename);
    if( fp == NULL){
        createError(error,functionName,"Can't open HashMap file",NULL,NULL);
        return NULL;
    }
    int* data = malloc(2 * sizeof(int));

    size_t lus = fread(data, sizeof(int), 2, fp);
    if(lus < 2){
        createError(error,functionName,"Didn't read 2 int value at the beginning of the file",NULL,NULL);
        free(data);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return data;
}