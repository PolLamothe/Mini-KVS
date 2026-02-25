#include "error.h"
#include "hashage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "persistence.h"


Entry* createEntry(char* table,int id,void* value,EntryValueType valueType,CachedEntry* next,CachedEntry* previous,Error** error){
    char* functionName = "hashage.createEntry";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }
    if(table == NULL){
        createError(error,functionName,"Table cannot be null",NULL,NULL);
        return NULL;
    }
    char* tableHeap = malloc((strlen(table)+1)*sizeof(char));
    if(tableHeap == NULL){
        createError(error,functionName,"Error during the allocation of the table",NULL,NULL);
        return NULL;
    }
    strcpy(tableHeap,table);

    Entry* result = malloc(sizeof(Entry));
    *result = (Entry){
        .id = id,
        .next = next,
        .previous = previous,
        .table = tableHeap,
        .valueType = valueType
    };

    if(value != NULL){
        int taille;
        if(valueType == STRING){
            taille = strlen((char*)value)+1;
        }else if(valueType == INT32){
            taille = sizeof(int32_t);
        }
        
        void* heapValue = malloc(taille);
        memcpy(heapValue, value, taille);

        result->value = heapValue;
    };
   
    return result;
}

void freeEntry(Entry* entry, Error** error){
    char* functionName = "hashage.freeEntry";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return;
    }
    if(entry == NULL){
        createError(error,functionName,"Entry cannot be null",NULL,NULL);
        return;
    }
    free(entry->table);
    free(entry);
}

HashMap* createHashMap(int dataSize,Error** error){
    char* functionName = "hashage.createHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }
    CachedEntry** data = calloc(dataSize,sizeof(CachedEntry*));

    if(data == NULL){
        createError(error,functionName,"Error during the allocation of data",NULL,NULL);
        return NULL;
    }

    HashMap* result = malloc(sizeof(HashMap));

    if(result == NULL){
        createError(error,functionName,"Error during the allocation of Hashmap null",NULL,NULL);
        return NULL;
    }

    *result = (HashMap){
        .data = data,
        .dataSize = dataSize
    };
    return result;
}

void freeCachedEntry(CachedEntry* cachedEntry,Error** error){
    char* functionName = "hashage.freeCachedEntry";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return;
    }
    if(cachedEntry != NULL){
        createError(error,functionName,"CachedEntry cannot be null",NULL,NULL);
        return;
    }
    if(cachedEntry->next != NULL){
        //We delete the linkage
        cachedEntry->next->previous = NULL;
    }
    if(cachedEntry->previous != NULL){
        cachedEntry->previous->next = NULL;
    }

    Error* deleteCachedEntryFromHashMapError = NULL;
    deleteCachedEntryFromHashMap(cachedEntry,&deleteCachedEntryFromHashMapError);
    if(deleteCachedEntryFromHashMapError != NULL){
        createError(error,functionName,"Error during the suppression of the entry from the hashMap",NULL,deleteCachedEntryFromHashMapError);
        return;
    }

    Error* deleteCachedEntryFromCachedHashMapError = NULL;
    deleteCachedEntryFromCachedHashMap(cachedEntry,&deleteCachedEntryFromCachedHashMapError);
    if(deleteCachedEntryFromCachedHashMapError != NULL){
        createError(error,functionName,"Error during the suppression of the entry from the CachedHashMap",NULL,deleteCachedEntryFromHashMapError);
        return;
    }

    Error* freeEntryError = NULL;
    freeEntry(cachedEntry->entry,&freeEntryError);
    if(freeEntryError != NULL){
        createError(error,functionName,"Error during the free of entry",NULL,freeEntryError);
        return;
    }
}

void freeHashMap(HashMap* hashMap,Error** error){
    char* functionName = "hashage.freeHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return;
    }
    if(hashMap == NULL){
        createError(error,functionName,"hashMap cannot be null",NULL,NULL);
        return;
    }
    for(int i = 0;i<hashMap->dataSize;i++){
        while(hashMap->data[i] != NULL){
            Error* freeCachedEntryError = NULL;
            freeCachedEntry(hashMap->data[i],freeCachedEntryError);
            if(freeCachedEntryError != NULL){
                createError(error,functionName,"Error during the free of the cachedEntry",NULL,freeCachedEntryError);
                return;
            }
        }
    }
    free(hashMap->data);
}

void freeCachedHashMap(CachedHashMap* cachedHashMap,Error** error){
    char* functionName = "hashage.freeCachedHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return;
    }
    if(cachedHashMap == NULL){
        createError(error,functionName,"CachedHashMap cannot be null",NULL,NULL);
        return;
    }
    Error* freeHashMapError = NULL;
    freeHashMap(cachedHashMap->hashMap,&freeHashMapError);
    if(freeHashMapError != NULL){
        createError(error,functionName,"Error during the free of the HashMap",NULL,freeHashMapError);
        return;
    }
}

void deleteCachedEntryFromHashMap(CachedEntry* cachedEntry,Error** error){
    char* functionName = "hashage.deleteCachedEntryFromHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return;
    }
    if(cachedEntry->cachedHashMap == NULL){
        createError(error,functionName,"CachedHashMap cannot be null",NULL,NULL);
        return;
    }
    if(cachedEntry->cachedHashMap->hashMap == NULL){
        createError(error,functionName,"HashMap cannot be null",NULL,NULL);
        return;
    }
    if(cachedEntry == NULL){
        createError(error,functionName,"cachedEntry cannot be null",NULL,NULL);
        return;
    }
    //If the entry is directly in the data
    if(cachedEntry->entry->previous == NULL){
        //If the entry is directly in the HashMap data
        Error** getHashMapCachedEntryPointerError = NULL;
        CachedEntry** cachedEntryPointer = getHashMapCachedEntryPointer(cachedEntry->cachedHashMap->hashMap,cachedEntry->entry,&getHashMapCachedEntryPointerError);
        if(getHashMapCachedEntryPointerError != NULL){
            createError(error,functionName,"Error during the fetch of the cachedEntry pointer",NULL,getHashMapCachedEntryPointerError);
            return;
        }
        if(*cachedEntryPointer == NULL){
            createError(error,functionName,"CachedEntry should be in HashMap data",NULL,NULL);
            return;
        }
        (*cachedEntryPointer) = cachedEntry->entry->next;
        if(cachedEntry->entry->next != NULL){
            cachedEntry->entry->next->previous = NULL;
        }
    }else{
        if(cachedEntry->entry->next != NULL){
            cachedEntry->entry->next->previous = cachedEntry->entry->previous;
        }
        cachedEntry->entry->previous->next = cachedEntry->entry->next;
    }
}

void deleteCachedEntryFromCachedHashMap(CachedEntry* cachedEntry,Error** error){
    char* functionName = "hashage.deleteCachedEntryFromCachedHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return;
    }
    if(cachedEntry->cachedHashMap == NULL){
        createError(error,functionName,"CachedHashMap cannot be null",NULL,NULL);
        return;
    }
    if(cachedEntry == NULL){
        createError(error,functionName,"cachedEntry cannot be null",NULL,NULL);
        return;
    }

    if(cachedEntry->previous != NULL){
        cachedEntry->previous->next = cachedEntry->next;
    }else{
        cachedEntry->cachedHashMap->firstCached = cachedEntry->next;
    }
    if(cachedEntry->next != NULL){
        cachedEntry->next->previous = cachedEntry->previous;
    }else{
        cachedEntry->cachedHashMap->lastCached = cachedEntry->previous;
    }

    cachedEntry->cachedHashMap->count--;
}

CachedHashMap** getHashMapCachedEntryPointer(HashMap* hashMap,Entry* entry,Error** error){
    char* functionName = "hashage.getHashMapCachedEntryPointer";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }
    if(hashMap == NULL){
        createError(error,functionName,"HashMap cannot be null",NULL,NULL);
        return NULL;
    }
    if(entry == NULL){
        createError(error,functionName,"Entry cannot be null",NULL,NULL);
        return NULL;
    }
    unsigned long index = hash(entry->table,entry->id,hashMap->dataSize);
    return &hashMap->data[index];
}

CachedHashMap* createCachedHashMap(int capacity,int dataSize,Error** error){
    char* functionName = "hashage.createCachedHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }

    Error* createHashMapError = NULL;
    HashMap* hashMap = createHashMap(dataSize,&createHashMapError);
    if(createHashMapError != NULL){
        createError(error,functionName,"Error during the creation of the hashMap",NULL,createHashMapError);
        return NULL;
    }
    CachedHashMap* result = malloc(sizeof(CachedHashMap));
    *result = (CachedHashMap){
        .capacity = capacity,
        .count = 0,
        .firstCached = NULL,
        .lastCached = NULL,
        .hashMap = hashMap
    };
    return result;
}

CachedHashMap* initHashMap(char* name,int dataSize,int cacheCapacity,Error** error){
    char* functionName = "hashage.initHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error not null",NULL,NULL);
        return NULL;
    }

    FILE* fp = readHashMapFile(name);
    if(fp == NULL){
        printf("No HashMap to import, creating it\n");
        Error* createCachedHashMapError = NULL;
        CachedHashMap* result = createCachedHashMap(cacheCapacity,dataSize,&createCachedHashMapError);
        if(createCachedHashMapError != NULL){
            createError(error,functionName,"Error during the creation of the cachedHashMap",NULL,createCachedHashMapError);
            return NULL;
        }

        Error* hashMapCreationErrror = NULL;
        createHashMapFile(name,result->hashMap,&hashMapCreationErrror);
        if (hashMapCreationErrror != NULL){
            createError(error,functionName,"Error during the creation of the HashMap file",NULL,hashMapCreationErrror);
            return NULL;
        }
        return result;
    }else{
        printf("Starting the importation of the existing HashMap\n");
        fclose(fp);

        Error* hashMapImportError = NULL;
        int hashMapDataSize = getHashMapDataSize(name,&hashMapImportError);
        if(hashMapImportError != NULL){
            createError(error,functionName,"Error during the import of the HashMap from file",NULL,hashMapImportError);
            return NULL;
        }

        Error* createCachedHashMapError = NULL;
        CachedHashMap* result = createCachedHashMap(cacheCapacity,dataSize,&createCachedHashMapError);
        if(createCachedHashMapError != NULL){
            createError(error,functionName,"Error during the creation of the cachedHashMap",NULL,createCachedHashMapError);
            return NULL;
        }

        Error* getFileScannerError = NULL;
        FileScanner* fileScanner = getFileScanner(name,&getFileScannerError);
        if(getFileScannerError != NULL){
            createError(error,functionName,"Error during the creation of the file scanner",NULL,getFileScannerError);
            return NULL;
        }

        int count = 0;
        while(1){
            Error* getNextFileEntryError = NULL;
            Entry* currentEntry = getNextFileEntry(fileScanner,&getNextFileEntryError);
            if(getNextFileEntryError != NULL){
                createError(error,functionName,"Error during the retrieval of the next file entry",NULL,getNextFileEntryError);
                return NULL;
            }
            if(currentEntry == NULL){
                break;
            }
            Error* addEntryInCachedHashMapError = NULL;
            addEntryInCachedHashMap(result,currentEntry,&addEntryInCachedHashMapError);
            count++;
            if(addEntryInCachedHashMapError != NULL){
                createError(error,functionName,"Error during the insertion of the entry in the cached hash map",NULL,addEntryInCachedHashMapError);
                return NULL;
            }
        }

        printf("%d entry imported into cache\n",count);
        return result;
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
    if (entry == NULL) {
        printf("Entry is NULL\n");
        return;
    }

    printf("┌────────────────────────────────────────┐\n");
    printf("│ Table: %-31s │\n", entry->table);
    printf("│ ID:    %-31d │\n", entry->id);
    
    if (entry->valueType == STRING) {
        printf("│ Type:  %-31s │\n", "STRING");
        printf("│ Value: %-31s │\n", (char*)entry->value);
    } else if (entry->valueType == INT32) {
        printf("│ Type:  %-31s │\n", "INT32");
        printf("│ Value: %-31d │\n", *(int32_t*)entry->value);
    }
    printf("└────────────────────────────────────────┘\n");
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
    CachedEntry* cachedEntry = malloc(sizeof(CachedEntry));
    *cachedEntry = (CachedEntry){
        .cachedHashMap = hashmap,
        .entry = entry
    };
    if(hashmap->firstCached == NULL){
        //If there is no entry in cache
        hashmap->firstCached = cachedEntry;
    }else{
        if(hashmap->count >= hashmap->capacity){
            //If the cache is full
            Error* removeCachedEntryError = NULL;
            removeCachedEntryFromCachedHashMap(hashmap,hashmap->lastCached,&removeCachedEntryError);
            if(removeCachedEntryError != NULL){
                createError(error,functionName,"Error during the suppression of a cached entry",NULL,removeCachedEntryError);
                free(cachedEntry);
                return;
            }
        }
        cachedEntry->next = hashmap->firstCached;
        hashmap->firstCached->previous = cachedEntry;
        hashmap->count++;
    }
    hashmap->lastCached = cachedEntry;

    Error* addEntryInHashMapError = NULL;
    addEntryInHashMap(hashmap->hashMap,cachedEntry,&addEntryInHashMapError);
    if(addEntryInHashMapError != NULL){
        createError(error,functionName,"Error during the insert in the hashMap",NULL,addEntryInHashMapError);
        free(cachedEntry);
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

CachedEntry* searchEntryInCachedHashMap(CachedHashMap* cachedHashMap,char* table, int id,Error** error){
    char* functionName = "hashage.searchEntryInCachedHashMap";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return NULL;
    }
    if(cachedHashMap == NULL){
        createError(error,functionName,"CachedHashMap cannot be null",NULL,NULL);
        return NULL;
    }
    if(table == NULL){
        createError(error,functionName,"table cannot be null",NULL,NULL);
        return NULL;
    }
    unsigned long index = hash(table,id,cachedHashMap->hashMap->dataSize);
    CachedEntry* current = cachedHashMap->hashMap->data[index];
    if(current == NULL){
        return NULL;
    }
    
    while(current != NULL){
        if(current->entry->id == id && strcmp(table,current->entry->table) == 0){
            return current;
        }
        current = current->next;
    }
    return NULL;
}