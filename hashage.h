#ifndef HASHAGE_H
#define HASHAGE_H
#include <stdio.h>
#include <string.h>
#include "error.h"

typedef struct Entry{
    char table[255];
    int id;
    void* value;
    struct Entry* next;
    struct Entry* previous;
} Entry;

typedef struct CachedEntry{
    Entry* entry;
    struct CachedEntry* previous;
    struct CachedEntry* next;
}CachedEntry;

typedef struct HashMap{
    Entry** data;
    int dataSize;
    size_t entrySize;
}HashMap;

typedef struct CachedHashMap{
    int count;
    int capacity;
    HashMap* hashMap;
}CachedHashMap;

CachedHashMap* createHashMap(char* name,int dataSize,int cacheCapacity,Error** error);

#endif