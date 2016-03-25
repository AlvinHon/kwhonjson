#ifndef _H_JPARSEEXPR
#define _H_JPARSEEXPR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

typedef struct{
    JParseElement* rules;
    int ruleLen;
    JParseElement result;
}JExprRule;

typedef struct{
    char checkc;
    JExprRule* exprRules;
    int numRules; // len of exprRules
}JExpression;

extern const JExpression jexpObj;
extern const JExpression jexpKey;
extern const JExpression jexpJval;
extern const JExpression jexpJvals;
extern const JExpression jexpJson;
#endif