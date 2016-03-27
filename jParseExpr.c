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

const JExprRule JExprRules[SIZE_JEXPRRULES]={
    {(JParseElement[]){JPE_STRTER,JPE_STRTER},2,JPE_OBJ}, // ".."
    {(JParseElement[]){JPE_COLUMN,JPE_OBJ},2,JPE_KEY}, // key: "..":
    {(JParseElement[]){JPE_OBJ,JPE_KEY},2,JPE_VAL}, // jval:  "..":".."
    
    // jvals: "..":"..",   |  "..":"..","..":".." | "..":"..","..":"..",
    {(JParseElement[]){JPE_SEPERATE,JPE_VAL},2,JPE_VALS},
    {(JParseElement[]){JPE_VAL,JPE_VALS},2,JPE_MULTIVALS},
    {(JParseElement[]){JPE_VALS,JPE_VALS},2,JPE_MULTIVALSS},
    
    {(JParseElement[]){JPE_CLOSE,JPE_VAL,JPE_OPEN},3,JPE_JSON},
    {(JParseElement[]){JPE_CLOSE,JPE_VALS,JPE_OPEN},3,JPE_JSON},
    {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALS,JPE_OPEN},3,JPE_JSON},
    {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALSS,JPE_OPEN},3,JPE_JSON},
    
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