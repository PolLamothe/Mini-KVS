#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "hashage.h"

typedef struct FileScanner{
    FILE* file;
}FileScanner;

FileScanner* getFileScanner(char* filename,Error** error);

void closeFileScanner(FileScanner* fileScanner,Error** error);

Entry* getNextFileEntry(FileScanner* fileScanner,Error** error);

FILE* readHashMapFile(char* name);

FILE* writeHashMapFile(char* name);

void createHashMapFile(char* filename,HashMap* hashmap,Error** error);

int getHashMapDataSize(char* filename,Error** error);

void insertEntry(char* filename,Entry* entry,Error** error);

Entry* searchEntryInFile(char* filename,char* table, int id,Error** error);

#endif