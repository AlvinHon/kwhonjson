#include "jParseExpr.h"

const char* JPENAME[] = {
    "JPE_OPEN",       // {
    "JPE_CLOSE",      // }
    "JPE_STRTER",     // "
    "JPE_SEPERATE",   // ,
    "JPE_COLUMN",     // :
    "JPE_OBJ",
    "JPE_VAL",
    "JPE_VALS",
    "JPE_MULTIVALS",
    "JPE_MULTIVALSS",
    "JPE_KEY",
    "JPE_JSON",
    "JPE_ANY",
    "JPE_NULL"
};

void _rulemakestring(JPEContainer* container, const char* str, int select){
    if(str == NULL)
        return;
    char** useptr = select == 0 ? &(container->partialKey): &(container->partialVal);
    if(useptr){
        free(*useptr);
        *useptr = NULL;
    }

    if(strlen(str) > 0){
        *useptr = (char*) malloc(strlen(str)+1);
        if(*useptr){
            memcpy(*useptr,str,strlen(str));
            (*useptr)[strlen(str)] = 0;
        }
    }
}

void ruleMakeKeyValue(JPEContainer* container, const char* keystr){
   _rulemakestring(container,keystr,container->selectKeyVal);
}

void ruleMakeSelect(JPEContainer* container, const char* keystr){
    container->selectKeyVal = 1;
}

void ruleMakeObj(JPEContainer* container, const char* keystr){
    if(!container || !container->partialKey || !container->partialVal){
        return;
    }
    JLinkedJsonObject* jlink = container->insertable;
    if(!jlink || !(*(jlink->obj))){
        return;
    }
    JsonSet((jlink->obj),container->partialKey,JSTRING, JsonString(container->partialVal));
    container->selectKeyVal = 0;
}

const JExprRule JExprRules[SIZE_JEXPRRULES]={
    {(JParseElement[]){JPE_STRTER,JPE_STRTER},2,JPE_OBJ,&ruleMakeKeyValue}, // ".."
    {(JParseElement[]){JPE_COLUMN,JPE_OBJ},2,JPE_KEY,&ruleMakeSelect}, // key: "..":
    {(JParseElement[]){JPE_OBJ,JPE_KEY},2,JPE_VAL,&ruleMakeObj}, // jval:  "..":".."
    
    // jvals: "..":"..",   |  "..":"..","..":".." | "..":"..","..":"..",
    {(JParseElement[]){JPE_SEPERATE,JPE_VAL},2,JPE_VALS,NULL},
    {(JParseElement[]){JPE_VAL,JPE_VALS},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_VALS,JPE_VALS},2,JPE_MULTIVALSS,NULL},
    
    {(JParseElement[]){JPE_CLOSE,JPE_VAL,JPE_OPEN},3,JPE_JSON,NULL},
    {(JParseElement[]){JPE_CLOSE,JPE_VALS,JPE_OPEN},3,JPE_JSON,NULL},
    {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALS,JPE_OPEN},3,JPE_JSON,NULL},
    {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALSS,JPE_OPEN},3,JPE_JSON,NULL},
    
};


// allhit: return 1 if rule check ok
const JExprRule* JResultFromExp(int (*allhit)(const JParseElement*,int,void**), void** checker){
    for(size_t i = 0;i<SIZE_JEXPRRULES;i++){
        const JExprRule* jr = &(JExprRules[i]);
        if(allhit(jr->rules,jr->ruleLen,checker)){
            return &(JExprRules[i]);
        }
        
    }
    return NULL;
}



JPEContainer* InitContainer(JsonObject** resJson){
  JPEContainer* ret = (JPEContainer*) malloc(sizeof(JPEContainer));
  if(ret){
    ret->rootJson = resJson;// MakeJsonObject();
    ret->insertable = (JLinkedJsonObject*) malloc(sizeof(JLinkedJsonObject));
    ret->partialKey = NULL;
    ret->partialVal = NULL;
    ret->selectKeyVal = 0;
  }
  if(!ret || !ret->rootJson || !ret->insertable){
    FreeContainer(&ret);
    ret = NULL;
  }
  ret->insertable->next = NULL;
  ret->insertable->obj = (ret->rootJson);
  return ret;
}

void FreeContainer(JPEContainer** container){
    if(*container){
        if((*container)->partialKey){
            free((*container)->partialKey);
        }
        if((*container)->partialVal){
            free((*container)->partialVal);
        }

        if((*container)->insertable){
            free((*container)->insertable);
        }

        free(*container);
    }
}