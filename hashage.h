#ifndef HASHAGE_H
#define HASHAGE_H
#include <stdio.h>
#include <string.h>
#include "error.h"

typedef enum EntryValueType{
    INT32,
    STRING,
}EntryValueType;

typedef struct Entry{
    char* table;
    int id;
    void* value;
    EntryValueType valueType;
    struct CachedEntry* next;
    struct CachedEntry* previous;
} Entry;

typedef struct CachedEntry{
    Entry* entry;
    struct CachedHashMap* cachedHashMap;
    struct CachedEntry* previous;
    struct CachedEntry* next;
}CachedEntry;

typedef struct HashMap{
    CachedEntry** data;
    int dataSize;
}HashMap;

typedef struct CachedHashMap{
    int count;
    int capacity;
    HashMap* hashMap;
    CachedEntry* firstCached;
    CachedEntry* lastCached;
}CachedHashMap;

CachedHashMap* initHashMap(char* name,int dataSize,int cacheCapacity,Error** error);

Entry* createEntry(char* table,int id,void* value,EntryValueType valueType,CachedEntry* next,CachedEntry* previous,Error** error);

void freeEntry(Entry* entry, Error** error);

HashMap* createHashMap(int dataSize,Error** error);

CachedHashMap* createCachedHashMap(int capacity,int dataSize,Error** error);

void freeHashMap(HashMap* hashMap,Error** error);

void freeCachedEntry(CachedEntry* cachedEntry,Error** error);

void deleteCachedEntryFromCachedHashMap(CachedEntry* cachedEntry,Error** error);

void deleteCachedEntryFromHashMap(CachedEntry* cachedEntry,Error** error);

CachedHashMap** getHashMapCachedEntryPointer(HashMap* hashMap,Entry* entry,Error** error);

unsigned long hash(char *table, int id, int capacity);

void verifyEntryForInsert(Entry* entry,Error** error);

void printEntry(Entry* entry);

void addEntryInCachedHashMap(CachedHashMap* hashmap,Entry* entry,Error** error);

void removeCachedEntryFromCachedHashMap(CachedHashMap* cachedHashmap,CachedEntry* cachedEntry,Error** error);

void removeEntryFromHashMap(HashMap* hashmap,Entry* entry,Error** error);

CachedEntry* searchEntryInCachedHashMap(CachedHashMap* cachedHashMap,char* table, int id,Error** error);

#endif