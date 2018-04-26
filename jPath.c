
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jPath.h"


PathStr* MakePath(char seperator, const char* path, int len){
    PathStr* ret = NULL;
    if(len > 0){
        int start = 0;
        for(int i = 0;i<len;i++){
            if(path[i] == seperator || i == (len-1)){
                int plen = i-start;
                if(i==(len-1))
                    plen++;
                if(plen > 0){
                    if(ret == NULL){ // first
                        ret = malloc(sizeof(PathStr));
                        ret->path = malloc(sizeof(char)*(plen+1));
                        ret->next = NULL;
                        strncpy_s(ret->path, sizeof(char)*(plen+1), path+start,plen);
                    }else{
                        PathStr* tail = ret;
                        while(tail->next != NULL){
                            tail = tail->next;
                        }
                        tail->next = malloc(sizeof(PathStr));
                        tail->next->path = malloc(sizeof(char)*(plen)+1);
                        tail->next->next = NULL;
                        strncpy_s(tail->next->path, sizeof(char)*(plen)+1, path+start,plen);
                    }
                    start = i+1;
                }
            }
        }
    }
    return ret;
}

void FreePath(PathStr* *path){
    if(path != NULL && *path != NULL){
        PathStr* pcur = *path;
        PathStr* tcut;
        while(pcur != NULL){
            tcut = pcur;
            pcur = pcur->next;
            free(tcut->path);
            free(tcut);
        }
    }
}