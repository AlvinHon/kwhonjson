#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aHEAP.h"

#define AHEAP_MALLOC    malloc
#define AHEAP_FREE      free
#define AHEAP_MEMCPY    memcpy

/*
void* testmalloc(size_t size){
    void* ret = malloc(size);
    printf("+ %4x %zu\n",ret,size);
    return ret;
}

void testfree(void* freeaddr){
    printf("- %4x\n",freeaddr);
    free(&(*freeaddr));
}
*/

ANode* MakeANode(void* key, void* content){
    ANode* ret = NULL;
    ret = AHEAP_MALLOC(sizeof(ANode));
    ret->left = NULL;
    ret->right = NULL;
    ret->parent = NULL;
    ret->key = key;
    ret->content = content;
    return ret;
}

AStackion* makeAStackion(void* content){
    AStackion* ret;
    ret = AHEAP_MALLOC(sizeof(AStackion));
    ret->content = content;
    ret->next = NULL;
    ret->prev = NULL;
    return ret;
}

AStack* MakeAStack(){
    AStack* ret;
    ret = AHEAP_MALLOC(sizeof(AStack));
    ret->root = NULL;
    ret->tail = NULL;
    return ret;
}

void FreeAStack(AStack* *stack, void (*cxtfree)(void**)){
    AStackion* tcur;
    AStackion* cur = (*stack)->root;
    if(cur != NULL){
        do{
            if(cxtfree != NULL)
                cxtfree(&(cur->content));
            tcur = cur;
            cur = cur->next;
            AHEAP_FREE(tcur);
        }while(cur);
    }
    AHEAP_FREE(*stack);
}

void PushAStack(AStack* *stack, void* content){
    AStackion* newstack = makeAStackion(content);
    AStackion* cur = (*stack)->root;
    if(cur == NULL){ // first element
        (*stack)->root = newstack;
        (*stack)->tail = newstack;
    }else{
        while(cur->next != NULL) cur = cur->next;
        cur->next = newstack;
        newstack->prev = cur;
        (*stack)->tail = newstack;
    }
}

int PopAStack(AStack* *stack, void* *content){
    AStackion* cur = (*stack)->tail;
    if(cur != NULL){
        if(cur == (*stack)->root){ // pop first element
            (*stack)->root = NULL;
            (*stack)->tail = NULL;
        }else{
            (*stack)->tail = cur->prev;
            if(cur->prev != NULL)
                cur->prev->next = NULL;
            if(content != NULL)
                *content = cur->content;
        }
        AHEAP_FREE(cur);
        return 1;
    }
    return 0;
}


DArray* MakeDArray(size_t elemsize){
    DArray* ret = (DArray*) AHEAP_MALLOC(sizeof(DArray));
    ret-> arr = AHEAP_MALLOC (2*elemsize);
    ret->elemsize = elemsize;
    ret->len = 0;
    ret->size = 2;
    return ret;
}

void FreeDArray(DArray** arrptr){
    if(*arrptr != NULL){
        AHEAP_FREE((*arrptr)->arr);
        AHEAP_FREE(*arrptr);
    }
}


int AddDArray(DArray** arrptr, char elem){
    if(*arrptr == NULL)
        return -1;
    if(((*arrptr)->len) +1 == (*arrptr)->size){ // expand
        void* narr = AHEAP_MALLOC((*arrptr)->elemsize * (*arrptr)->size * 2);
        if(narr == NULL)
            return -1;
        AHEAP_MEMCPY(narr,(*arrptr)->arr,(*arrptr)->len*(*arrptr)->elemsize);
        AHEAP_FREE((*arrptr)->arr);
        (*arrptr)->arr = narr;
        (*arrptr)->size *= 2;
    }

    // add
    (*arrptr)->arr[(*arrptr)->len] = elem;
    (*arrptr)->len ++;
    return (*arrptr)->len;

}


void ReDArray(DArray** arrptr){
    size_t start;
    size_t end;
    char swap;
    if(*arrptr != NULL && (*arrptr)->len > 1){
        start = 0;
        end = (*arrptr)->len -1;
        while(start < end){
            swap = (*arrptr)->arr[start];
            (*arrptr)->arr[start] = (*arrptr)->arr[end];
            (*arrptr)->arr[end] = swap;
            start ++;
            end --;
        }
    }
}

void PrinfFDArray(DArray** arrptr, FILE* f){
    int i;
    if(*arrptr != NULL && (*arrptr)->len > 0){
        for(i=0;i<(*arrptr)->len;i++){
            fprintf(f,"%c",(*arrptr)->arr[i]);
        }
    }
    fprintf(f,"\n");
}
