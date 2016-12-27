#include "jParseExpr.h"

const char* JPENAME[] = {
    "JPE_OPEN",       // {
    "JPE_CLOSE",      // }
    "JPE_STRTER",     // "
    "JPE_SEPERATE",   // ,
    "JPE_COLUMN",     // :
    "JPE_AOPEN",      // [
    "JPE_ACLOSE",     // ]
    "JPE_OBJ",
    "JPE_VAL",
    "JPE_JVAL",
    "JPE_MULTIVALS",
    "JPE_KEY",
    "JPE_JSON",
    "JPE_ANY",
    "JPE_NULL"
};

static void _rulemakestring(JPEContainer* container, const char* str, int select){
    if(str == NULL)
        return;
    char** useptr = select == SelectPartialKey ? &(container->partialKey): &(container->partialVal);
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

static void _objSetAdd(JPEContainer* container, const char* key, JValType type, void* val){
    if(container->tmpObjType == JSONOBJ){
        JsonSet(&(container->tmpObj.tmpJson),key,type,val);
    }else if(container->tmpObjType == JSONARR){
        JsonAdd(&(container->tmpObj.tmpArr),key,type,val);
    }
}

void ruleMakeKeyValue(JPEContainer* container, const char* keystr){
   _rulemakestring(container,keystr,container->selectKeyVal);
}

void ruleMakeSelect(JPEContainer* container, const char* keystr){
    container->selectKeyVal = SelectPartialVal;
}

void ruleMakeObj(JPEContainer* container, const char* keystr){
    if(!container || !container->partialKey || !container->partialVal){
        return;
    }

    _objSetAdd(container,container->partialKey,JSTRING, JsonString(container->partialVal));
    container->selectKeyVal = SelectPartialKey; // reset state
}

void ruleMakeObjWoQuotes(JPEContainer* container, const char* keystr){
    if(!container || !container->partialKey || keystr == NULL){
        return;
    }

    container->selectKeyVal = SelectPartialVal;
    _rulemakestring(container,keystr,container->selectKeyVal);
    if(IsStrNumber(keystr)){
        _objSetAdd(container,container->partialKey,JINTEGER, JsonInteger(atoi(keystr)));
    }else if (strlen(keystr) > 0){
        _objSetAdd(container,container->partialKey,JSTRING, JsonString(container->partialVal));
    }else{
        _objSetAdd(container,container->partialKey,JNULL, NULL);
    }
    container->selectKeyVal = SelectPartialKey;
}

void ruleJsonLevelUp(JPEContainer* container, const char* keystr){
    if(!container || !container->partialKey){
        return;
    }

    // setup json object and linkage info
    JsonObject* nextJobj = MakeJsonObject();
    if(!nextJobj){
        return;
    }

    // setup next level json object
    _objSetAdd(container,container->partialKey,JSONOBJ,nextJobj);
    container->tmpObj.tmpJson = nextJobj;
    container->tmpObjType = JSONOBJ;
    container->selectKeyVal = SelectPartialKey;    
}

void ruleArrLevelUp(JPEContainer* container, const char* keystr){
    if(!container || !container->partialKey){
        return;
    }

    JsonArray* nextJarr = MakeJArray();
    if(!nextJarr){
        return;
    }
    _objSetAdd(container,container->partialKey,JSONARR,nextJarr);
    container->tmpObj.tmpArr = nextJarr;
    container->tmpObjType = JSONARR;
    container->selectKeyVal = SelectPartialKey;
}

void ruleLevelDown(JPEContainer* container, const char* keystr){
    if(!container){
        return;
    }

    if(container->tmpObjType == JSONOBJ){
        if(container->tmpObj.tmpJson->root != NULL){
            if(container->tmpObj.tmpJson->rootType == JSONOBJ){
                container->tmpObj.tmpJson = container->tmpObj.tmpJson->root;
                container->tmpObjType = JSONOBJ;
            }else{
                container->tmpObj.tmpArr = container->tmpObj.tmpJson->root;
                container->tmpObjType = JSONARR;
            }
        }
    }else{
        if(container->tmpObj.tmpArr->root != NULL){
            if(container->tmpObj.tmpArr->rootType == JSONOBJ){
                container->tmpObj.tmpJson = container->tmpObj.tmpArr->root;
                container->tmpObjType = JSONOBJ;
            }else{
                container->tmpObj.tmpArr = container->tmpObj.tmpArr->root;
                container->tmpObjType = JSONARR;
            }
        }
    }

}

void ruleMakeClosingObjWoQuotes(JPEContainer* container, const char* keystr){
    ruleMakeObjWoQuotes(container,keystr);
    ruleLevelDown(container,keystr);
}

const JExprRule JExprRules[SIZE_JEXPRRULES]={
    {(JParseElement[]){JPE_STRTER,JPE_STRTER},2,JPE_OBJ,&ruleMakeKeyValue}, // ".."
    {(JParseElement[]){JPE_COLUMN,JPE_OBJ},2,JPE_KEY,&ruleMakeSelect}, // key: "..":
    {(JParseElement[]){JPE_OBJ,JPE_KEY},2,JPE_VAL,&ruleMakeObj}, // jval:  "..":".."
    {(JParseElement[]){JPE_SEPERATE,JPE_KEY},2,JPE_VAL,&ruleMakeObjWoQuotes}, // jval: "..":.., (force to make int/string)
    {(JParseElement[]){JPE_CLOSE,JPE_KEY,JPE_OPEN,JPE_KEY},4,JPE_JSON,&ruleMakeClosingObjWoQuotes}, // jval: "..":..} (force to make int/string)
    {(JParseElement[]){JPE_ACLOSE,JPE_KEY,JPE_AOPEN,JPE_KEY},4,JPE_JSON,&ruleMakeClosingObjWoQuotes}, // jval: "..":..] (force to make int/string)
    {(JParseElement[]){JPE_CLOSE,JPE_KEY},2,JPE_VAL,&ruleMakeClosingObjWoQuotes},
    {(JParseElement[]){JPE_ACLOSE,JPE_KEY},2,JPE_VAL,&ruleMakeClosingObjWoQuotes},
    {(JParseElement[]){JPE_OPEN,JPE_KEY},2,JPE_NULL,&ruleJsonLevelUp}, 
    {(JParseElement[]){JPE_AOPEN,JPE_KEY},2,JPE_NULL,&ruleArrLevelUp}, 
   
    {(JParseElement[]){JPE_JSON,JPE_KEY},2,JPE_JVAL,NULL}, 
    {(JParseElement[]){JPE_VAL,JPE_VAL},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_JVAL,JPE_VAL},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_VAL,JPE_JVAL},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_JVAL,JPE_JVAL},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_SEPERATE,JPE_VAL},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_SEPERATE,JPE_JVAL},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_SEPERATE,JPE_MULTIVALS},2,JPE_MULTIVALS,NULL}, // !! weird case: {"a":"b",,,,,}
    {(JParseElement[]){JPE_JVAL,JPE_MULTIVALS},2,JPE_MULTIVALS,NULL},
    {(JParseElement[]){JPE_VAL,JPE_MULTIVALS},2,JPE_MULTIVALS,NULL},
    
    {(JParseElement[]){JPE_ACLOSE,JPE_VAL,JPE_AOPEN},3,JPE_JSON,&ruleLevelDown},
    {(JParseElement[]){JPE_ACLOSE,JPE_JSON,JPE_AOPEN},3,JPE_JSON,&ruleLevelDown},
    {(JParseElement[]){JPE_ACLOSE,JPE_MULTIVALS,JPE_AOPEN},3,JPE_JSON,&ruleLevelDown},
    {(JParseElement[]){JPE_CLOSE,JPE_VAL,JPE_OPEN},3,JPE_JSON,&ruleLevelDown},
    {(JParseElement[]){JPE_CLOSE,JPE_JSON,JPE_OPEN},3,JPE_JSON,&ruleLevelDown},
    {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALS,JPE_OPEN},3,JPE_JSON,&ruleLevelDown},
    
};


// allhit: return 1 if rule check ok
const JExprRule* JResultFromExp(int (*allhit)(const JParseElement*,int,void**), void** checker){
    for(size_t i = 0;i<SIZE_JEXPRRULES;i++){
        const JExprRule* jr = &(JExprRules[i]);
        if(allhit(jr->rules,jr->ruleLen,checker)){
            
            printf("rule: ");
            for(size_t j=0;j<jr->ruleLen;j++){
                printf("%s,",JPENAME[jr->rules[j]]);
            }
            printf("->%s\n",JPENAME[jr->result]);
            
            return &(JExprRules[i]);
        }
    }
    return NULL;
}

JPEContainer* InitContainer(JsonObject** resJson){
  JPEContainer* ret = (JPEContainer*) malloc(sizeof(JPEContainer));
  if(ret){
    ret->rootJson = resJson;// MakeJsonObject();
    ret->partialKey = NULL;
    ret->partialVal = NULL;
    ret->tmpObj.tmpJson = *(ret->rootJson);
    ret->tmpObjType = JSONOBJ;
    ret->selectKeyVal = 0;
  }
  if(!ret || !ret->rootJson ){
    FreeContainer(&ret);
    ret = NULL;
  }
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
        free(*container);
    }
}