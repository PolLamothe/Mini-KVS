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

    fwrite(&hashmap->dataSize,sizeof(int),1,fp);
    fclose(fp);
    return;
}

int getHashMapDataSize(char* filename,Error** error){
    char* functionName = "persistence.getHashMapDataSize";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return -1;
    }

    FILE* fp = readHashMapFile(filename);
    if( fp == NULL){
        createError(error,functionName,"Can't open HashMap file",NULL,NULL);
        return -1;
    }
    int data;

    size_t lus = fread(&data, sizeof(int), 1, fp);
    if(lus != 1){
        char debug[255];
        sprintf(debug,"%d int read\n",(int)lus);
        createError(error,functionName,"Didn't read 2 int value at the beginning of the file",debug,NULL);
        fclose(fp);
        return -1;
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
    int valueSize;
    if(entry->valueType == INT32){
        valueSize = sizeof(int32_t);
    }else if(entry->valueType == STRING){
        valueSize = (int)(strlen(entry->value)+1);
    }

    int entrySize = strlen(entry->table)+1+sizeof(int)+sizeof(int)+valueSize;
    if(!fwrite(&entrySize,sizeof(int),1,fp)){
        createError(error,functionName,"Entry size   wasn't written",NULL,NULL);
        fclose(fp);
        return;
    }
    if(!fwrite(entry->table,strlen(entry->table)+1,1,fp)){
        createError(error,functionName,"Table name wasn't written",NULL,NULL);
        fclose(fp);
        return;
    };
    if(!fwrite(&(entry->id),sizeof(int),1,fp)){
        createError(error,functionName,"ID wasn't written",NULL,NULL);
        fclose(fp);
        return;
    }
    int valueType = (int)entry->valueType;
    if(!fwrite(&valueType,sizeof(int),1,fp)){
        createError(error,functionName,"Value type wasn't written",NULL,NULL);
        fclose(fp);
        return;
    }
    if(!fwrite(entry->value,valueSize,1,fp)){
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
    FILE* fp = readHashMapFile(filename);
    if(fp == NULL){
        createError(error,functionName,"File can't be null",NULL,NULL);
        fclose(fp);
        return NULL;
    }
    fseek(fp,sizeof(int),SEEK_SET);//We put the cursor after the file header

    while(1){
        //For every entry in the file
        int entrySize;
        fread(&entrySize,sizeof(int),1,fp);
        if (feof(fp)) break;
        long adressSave = ftell(fp);
        char tableName[255];
        int i = 0;
        char c;
        bool interrupted = false;
        while (1) {
            //For every char in the table name in the file
            c = fgetc(fp);
            if(i > strlen(table)+1 || c == EOF){
                interrupted = true;
                break;
            }
            tableName[i] = c;
            if (i == 255){
                interrupted = true;
                break;
            }
            if (table[i] != c){
                interrupted = true;
                break;
            }
            if(table[i] == '\0'){
                break;
            }
            i++;
        }
        if(interrupted){
            fseek(fp,adressSave+entrySize,SEEK_SET);
            continue;
        }
        int readId;
        fread(&readId,sizeof(int),1,fp);
        if(readId == id){
            Entry* result = (Entry*)calloc(sizeof(Entry),1);
            EntryValueType valueType;
            fread(&valueType,sizeof(int),1,fp);

            result->id = id;
            result->table = table;
            result->valueType = valueType;

            if(valueType == INT32){
                int32_t* value = malloc(sizeof(int32_t));
                fread(value,sizeof(int32_t),1,fp);
                result->value = value;
            }else if(valueType == STRING){
                char value[255];
                int i = 0;
                char c = fgetc(fp);
                while (c != EOF) {
                    value[i] = c;
                    i++;
                    if(c == '\0'){
                        break;
                    }
                    c = fgetc(fp);
                }
                char* cleanedValue = malloc((strlen(value)+1)*sizeof(char));
                if(cleanedValue == NULL){
                    createError(error,functionName,"Cleaned value memory allocation failed",NULL,NULL);
                    free(result);
                    fclose(fp);
                    return NULL;
                }
                strcpy(cleanedValue, value);
                result->value = cleanedValue;
            }else{
                createError(error,functionName,"Invalid value type",NULL,NULL);
                free(result);
                fclose(fp);
                return NULL;
            }
            return result;
        }else{
            fseek(fp,adressSave+entrySize,SEEK_SET);
            continue;
        }
    }
    return NULL;
}