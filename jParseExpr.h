#ifndef _H_JPARSEEXPR
#define _H_JPARSEEXPR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tJSON.h"
#include "jUtil.h"


typedef enum{
    JPE_OPEN = 0,   // {
    JPE_CLOSE,      // }
    JPE_STRTER,     // "
    JPE_SEPERATE,   // ,
    JPE_COLUMN,     // :
    JPE_AOPEN,      // [
    JPE_ACLOSE,     // ]
    JPE_OBJ,
    JPE_VAL,
    JPE_JVAL,
    JPE_AOPENVAL,
    JPE_VALCLOSED,
    JPE_VALACLOSED,
    JPE_MULTIVALS,
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

typedef enum{
    SelectPartialKey=0,
    SelectPartialVal
}SelectPartialType;

typedef struct{
  JsonObject** rootJson; // point to resulting jsonobject
  union{
      JsonObject* tmpJson;
      JsonArray* tmpArr;
  }tmpObj; // point to current jsonobject
  JValType tmpObjType;
  char* partialKey;
  char* partialVal;
  SelectPartialType selectKeyVal;
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
#define SIZE_JEXPRRULES 38
extern const JExprRule JExprRules[];

const JExprRule* JResultFromExp(int (*allhit)(const JParseElement*,int,void**), void** checker);



JPEContainer* InitContainer(JsonObject** resJson);
void FreeContainer(JPEContainer** container);

#endif