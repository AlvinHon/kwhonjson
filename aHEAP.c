#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aHEAP.h"

#define AHEAP_MALLOC    malloc
#define AHEAP_FREE      free

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
    ret->head = NULL;
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
        (*stack)->head = newstack;
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
        (*stack)->tail = cur->prev;
        if(cur->prev != NULL)
            cur->prev->next = NULL;
        *content = cur->content;
        AHEAP_FREE(cur);
        return 1;
    }
    return 0;
}
