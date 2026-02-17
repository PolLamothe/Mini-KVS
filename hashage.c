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
    printf("Table : %s\nKey : %d\nValue : %s\n",entry->table,entry->id,(char*)entry->value);
}


unsigned long hash(char *table, int id, int capacity) {
    unsigned long hash = 5381;
    int c;

    while ((c = *table++)) {
        hash = ((hash << 5) + hash) + c;
    }

    hash = ((hash << 5) + hash) + id;

    return hash % capacity;
}

void addEntryInHashMap(HashMap* hashmap,CachedEntry* entry,Error** error){
    char* functionName = "hashage.addEntryInHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }
    if(hashmap == NULL){
        createError(error,functionName,"Hashmap cannot be null",NULL,NULL);
        return;
    }
    if(entry == NULL){
        createError(error,functionName,"Entry cannot be null",NULL,NULL);
        return;
    }
    unsigned long index = hash(entry->entry->table,entry->entry->id,hashmap->dataSize);
    if(hashmap->data[index] == NULL){
        hashmap->data[index] = entry;
    }else{
        hashmap->data[index]->previous = entry;
        entry->next = hashmap->data[index];
        hashmap->data[index] = entry;
    }
}

void addEntryInCachedHashMap(CachedHashMap* hashmap,Entry* entry,Error** error){
    char* functionName = "hashage.addEntryInCachedHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }
    if(hashmap == NULL){
        createError(error,functionName,"CachedHashMap cannot null",NULL,NULL);
        return;
    }
    if(entry == NULL){
        createError(error,functionName,"Entry cannot null",NULL,NULL);
        return;
    }
    CachedEntry* cachedEntry;
    if(hashmap->firstCached == NULL){
        //If there is no entry in cache
        cachedEntry = (CachedEntry*)malloc(sizeof(CachedEntry));
        hashmap->firstCached = cachedEntry;
        hashmap->lastCached = hashmap->firstCached;
        hashmap->firstCached->entry = entry;
    }else{
        if(hashmap->count >= hashmap->capacity){
            //If the cache is full
            Error* removeCachedEntryError = NULL;
            removeCachedEntryFromCachedHashMap(hashmap,hashmap->lastCached,&removeCachedEntryError);
            if(removeCachedEntryError != NULL){
                createError(error,functionName,"Error during the suppression of a cached entry",NULL,removeCachedEntryError);
                return;
            }
        }
        cachedEntry = (CachedEntry*)malloc(sizeof(CachedEntry));
        cachedEntry->entry = entry;
        cachedEntry->next = hashmap->firstCached;
        hashmap->firstCached->previous = cachedEntry;
        hashmap->count++;
    }
    Error* addEntryInHashMapError = NULL;
    addEntryInHashMap(hashmap->hashMap,cachedEntry,&addEntryInHashMapError);
    if(addEntryInHashMapError != NULL){
        createError(error,functionName,"Error during the insert in the hashMap",NULL,addEntryInHashMapError);
        return;
    }
}

void removeCachedEntryFromCachedHashMap(CachedHashMap* cachedHashmap,CachedEntry* cachedEntry,Error** error){
    char* functionName = "hashage.removeCachedEntryFromCachedHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }
    if(cachedHashmap == NULL){
        createError(error,functionName,"CachedHashMap cannot be null",NULL,NULL);
        return;
    }
    if(cachedEntry == NULL){
        createError(error,functionName,"CachedEntry cannot be null",NULL,NULL);
        return;
    }
    if(cachedHashmap->firstCached == cachedEntry){
        cachedHashmap->firstCached = cachedEntry->next;
    }
    if(cachedHashmap->lastCached == cachedEntry){
        cachedHashmap->lastCached = cachedEntry->previous;
    }
    if(cachedEntry->previous != NULL){
        cachedEntry->previous->next = cachedEntry->next;
    }
    if(cachedEntry->next != NULL){
        cachedEntry->next->previous = cachedEntry->previous;
    
    }
    cachedHashmap->count--;
    Error* removeEntryError = NULL;
    removeEntryFromHashMap(cachedHashmap->hashMap,cachedEntry->entry,&removeEntryError);
    if(removeEntryError != NULL){
        createError(error,functionName,"Error during the suppression of entry",NULL,removeEntryError);
        return;
    }
}

void removeEntryFromHashMap(HashMap* hashmap,Entry* entry,Error** error){
    char* functionName = "hashage.removeEntryFromHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }
    if(hashmap == NULL){
        createError(error,functionName,"Hashmap cannot be null",NULL,NULL);
        return;
    }
    if(entry == NULL){
        createError(error,functionName,"Entry cannot be null",NULL,NULL);
        return;
    }
    if(entry->previous == NULL){
        //Is directly in the hashMap data
        unsigned long index = hash(entry->table,entry->id,hashmap->dataSize);
        if(hashmap->data[index]->entry != entry){
            createError(error,functionName,"Entry found in hashMap isn't equal to the one provided",NULL,NULL);
            return;
        }
        hashmap->data[index] = NULL;
    }
    if(entry->next != NULL){
        //If another Entry was inserted after
        entry->next->previous = NULL;
        if(entry->previous == NULL){
            //Is directly in the HashMap data so we will replace our entry by the one after
            unsigned long index = hash(entry->table,entry->id,hashmap->dataSize);
            hashmap->data[index] = entry->next;
            entry->next->previous = NULL;
        }else{
            //Our entry is inserted after another entry so we will attach the entry before and the entry after
            entry->next->previous = entry->previous;
            entry->previous->next = entry->next;
        }
    }
    free(entry->table);
    free(entry->value);
    free(entry);
}