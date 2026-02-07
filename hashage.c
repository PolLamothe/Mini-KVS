#include "error.h"
#include "hashage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "persistence.h"

CachedHashMap* createHashMap(char* name,int dataSize,int cacheCapacity,Error** error){
    char* functionName = "hashage.createHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }

    FILE* fp = readHashMapFile(name);
    if(fp == NULL){
        printf("No HashMap to import, creating it\n");
        HashMap* hashmap = malloc(sizeof(HashMap));
        CachedHashMap* cachedHashMap = malloc(sizeof(CachedHashMap));
        CachedEntry** data = malloc(sizeof(CachedEntry*)*dataSize);

        *cachedHashMap = (CachedHashMap){
            .hashMap = hashmap,
            .count = 0,
            .capacity = cacheCapacity,
        };
        *hashmap = (HashMap){
            .dataSize = dataSize,
            .entrySize = sizeof(CachedEntry),
            .data = data
        };
        Error* hashMapCreationErrror = NULL;
        createHashMapFile(name,hashmap,&hashMapCreationErrror);
        if (hashMapCreationErrror != NULL){
            createError(error,functionName,"Error during the creation of the HashMap file",NULL,hashMapCreationErrror);
            return NULL;
        }
        return cachedHashMap;
    }else{
        printf("Starting the importation of the existing HashMap\n");
        HashMap* hashmap = malloc(sizeof(HashMap));
        CachedHashMap* cachedHashMap = malloc(sizeof(CachedHashMap));

        Error* hashMapImportError = NULL;
        int* importedData = importHashMapFromFile(name,&hashMapImportError);
        if(hashMapImportError != NULL){
            createError(error,functionName,"Error during the import of the HashMap from file",NULL,hashMapImportError);
            fclose(fp);
            return NULL;
        }

        CachedEntry** data = malloc(sizeof(CachedEntry*)*importedData[1]);
         *cachedHashMap = (CachedHashMap){
            .hashMap = hashmap,
            .count = 0,
            .capacity = cacheCapacity,
        };
        *hashmap = (HashMap){
            .dataSize = importedData[1],
            .entrySize = importedData[0],
            .data = data
        };
        fclose(fp);
        return cachedHashMap;
    }
    return NULL;
}

void verifyEntryForInsert(Entry* entry,Error** error){
    char* functionName = "hashage.verifyEntry";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }
    if(entry == NULL){
        createError(error,functionName,"Entry must point to an address",NULL,NULL);
        return;
    }
    if(entry->id == -1){
        createError(error,functionName,"Entry id can't be null",NULL,NULL);
        return;
    }
    if(entry->value == NULL){
        createError(error,functionName,"Entry value can't be null",NULL,NULL);
        return;
    }
    if(strcmp(entry->table,"") == 0){
        createError(error,functionName,"Entry table can't be null",NULL,NULL);
        return;
    }
    return;
}

void printEntry(Entry* entry){
    printf("Table : %s\nKey : %d\nValue : %s\n",entry->table,entry->id,entry->value);
}