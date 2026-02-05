#include "error.h"
#include "hashage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "persistence.h"

CachedHashMap* createHashMap(char* name,int dataSize,int cacheCapacity,Error** error){
    char functionName[] = "hashage.createHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "%s.db", name);

    FILE* fp = openHashMapFile(filename);
    if(fp == NULL){
        printf("No HashMap to import, creating it\n");
        HashMap* hashmap = malloc(sizeof(HashMap));
        CachedHashMap* cachedHashMap = malloc(sizeof(CachedHashMap));
        Entry** data = malloc(sizeof(Entry*)*dataSize);

        *cachedHashMap = (CachedHashMap){
            .hashMap = hashmap,
            .count = 0,
            .capacity = cacheCapacity,
        };
        *hashmap = (HashMap){
            .dataSize = dataSize,
            .entrySize = sizeof(Entry),
            .data = data
        };
        Error* hashMapCreationErrror = NULL;
        createHashMapFile(filename,hashmap,&hashMapCreationErrror);
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
        int* importedData = importHashMapFromFile(filename,&hashMapImportError);
        if(hashMapImportError != NULL){
            createError(error,functionName,"Error during the import of the HashMap from file",NULL,hashMapImportError);
            return NULL;
        }

        Entry** data = malloc(sizeof(Entry*)*importedData[1]);
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
        return cachedHashMap;
    }
    return NULL;
}