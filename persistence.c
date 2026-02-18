#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hashage.h"
#include "persistence.h"

FILE* readHashMapFile(char* filename){
    FILE* fp = fopen(filename, "rb");
    if(!fp){
        return NULL;
    }
    return fp;
}

FILE* writeHashMapFile(char* filename){
    FILE* fp = fopen(filename, "a");
    if(!fp){
        return NULL;
    }
    return fp;
}

void createHashMapFile(char* filename,HashMap* hashmap,Error** error){
    char* functionName = "persistence.createHashMapFile";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }

    FILE* fp = readHashMapFile(filename);
    if( fp != NULL){
        fclose(fp);
        createError(error,functionName,"File already exist",NULL,NULL);
        return;
    }
    fp = writeHashMapFile(filename);

    int entrySize = (int)hashmap->entrySize;
    fwrite(&entrySize,sizeof(int),1,fp);
    fwrite(&hashmap->dataSize,sizeof(int),1,fp);
    fclose(fp);
    return;
}

int* importHashMapFromFile(char* filename,Error** error){
    char* functionName = "persistence.importHashMapFromFile";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return NULL;
    }

    FILE* fp = writeHashMapFile(filename);
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

void insertEntry(char* filename,Entry* entry,Error** error){
    char* functionName = "persistence.insertEntry";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }
    FILE* fp = writeHashMapFile(filename);
    if(fp == NULL){
        createError(error,functionName,"File can't be null",NULL,NULL);
        fclose(fp);
        return;
    }
    Error* entryValidationError = NULL;
    verifyEntryForInsert(entry,&entryValidationError);
    if(entryValidationError != NULL){
        createError(error,functionName,"Entry not valid",NULL,entryValidationError);
        fclose(fp);
        return;
    }

    if(!fwrite(entry->table,strlen(entry->table),1,fp)){
        createError(error,functionName,"Table name wasn't written",NULL,NULL);
        fclose(fp);
        return;
    };
    if(!fwrite(&(entry->id),sizeof(int),1,fp)){
        createError(error,functionName,"ID wasn't written",NULL,NULL);
        fclose(fp);
        return;
    }
    if(!fwrite(entry->value,entry->valueSize,1,fp)){
        createError(error,functionName,"Value wasn't written",NULL,NULL);
        fclose(fp);
        return;
    }
    fclose(fp);
}

Entry* searchEntryInFile(char* filename,char* table, int id,Error** error){
    char* functionName = "persistence.searchEntryInFile";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return NULL;
    }
    FILE* fp = writeHashMapFile(filename);
    if(fp == NULL){
        createError(error,functionName,"File can't be null",NULL,NULL);
        fclose(fp);
        return NULL;
    }
}