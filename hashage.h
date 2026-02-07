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
    struct Entry* next;
    struct Entry* previous;
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
}CachedHashMap;

CachedHashMap* createHashMap(char* name,int dataSize,int cacheCapacity,Error** error);

void verifyEntryForInsert(Entry* entry,Error** error);

void printEntry(Entry* entry);

#endif