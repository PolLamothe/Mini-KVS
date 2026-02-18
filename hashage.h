#ifndef HASHAGE_H
#define HASHAGE_H
#include <stdio.h>
#include <string.h>
#include "error.h"

typedef struct Entry{
    char* table;
    int id;
    void* value;
    size_t valueSize;
    struct CachedEntry* next;
    struct CachedEntry* previous;
} Entry;

typedef struct Table{
    char name;
    int count;
}Table;

typedef struct CachedEntry{
    Entry* entry;
    struct CachedEntry* previous;
    struct CachedEntry* next;
}CachedEntry;

typedef struct HashMap{
    CachedEntry** data;
    int dataSize;
    size_t entrySize;
}HashMap;

typedef struct CachedHashMap{
    int count;
    int capacity;
    HashMap* hashMap;
    CachedEntry* firstCached;
    CachedEntry* lastCached;
}CachedHashMap;

CachedHashMap* createHashMap(char* name,int dataSize,int cacheCapacity,Error** error);

void verifyEntryForInsert(Entry* entry,Error** error);

void printEntry(Entry* entry);

void addEntryInCachedHashMap(CachedHashMap* hashmap,Entry* entry,Error** error);

void removeCachedEntryFromCachedHashMap(CachedHashMap* cachedHashmap,CachedEntry* cachedEntry,Error** error);

void removeEntryFromHashMap(HashMap* hashmap,Entry* entry,Error** error);

CachedEntry* searchEntryInCachedHashMap(CachedHashMap* cachedHashMap,char* table, int id,Error** error);

#endif