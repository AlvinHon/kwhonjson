#ifndef _H_JPARSEEXPR
#define _H_JPARSEEXPR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tJSON.h"


typedef enum{
    JPE_OPEN = 0,   // {
    JPE_CLOSE,      // }
    JPE_STRTER,     // "
    JPE_SEPERATE,   // ,
    JPE_COLUMN,     // :
    JPE_OBJ,
    JPE_VAL,
    JPE_VALS,
    JPE_MULTIVALS,
    JPE_MULTIVALSS,
    JPE_KEY,
    JPE_JSON,
    JPE_ANY,
    JPE_NULL
}JParseElement;

#define JLinkedJsonObject struct t_JLinkedJsonObject
struct t_JLinkedJsonObject{
    JLinkedJsonObject* next;
    JsonObject** obj;
};

typedef struct{
  JsonObject** rootJson;
  JLinkedJsonObject* insertable;
  char* partialKey;
  char* partialVal;
  int selectKeyVal;
}JPEContainer;

#pragma pack(push,1)
typedef struct{
    JParseElement* rules;
    int ruleLen;
    JParseElement result;
    void (*ruleMakeJson)(JPEContainer*,const char*);
}JExprRule;
#pragma pack(pop)


typedef struct{
    char checkc;
    JExprRule* exprRules;
    int numRules; // len of exprRules
}JExpresion;


extern const char* JPENAME[];
#define SIZE_JEXPRRULES 10
extern const JExprRule JExprRules[];

const JExprRule* JResultFromExp(int (*allhit)(const JParseElement*,int,void**), void** checker);



JPEContainer* InitContainer(JsonObject** resJson);
void FreeContainer(JPEContainer** container);

#endif