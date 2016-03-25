#include "jParseExpr.h"

const  JExpression jexpObj ={
  .checkc = '"',
  .exprRules = (JExprRule[]){
      {(JParseElement[]){JPE_STRTER,JPE_STRTER},2,JPE_OBJ},
  },
  .numRules = 1
};
const JExpression jexpKey={
    .checkc = ':',
    .exprRules = (JExprRule[]){
        {(JParseElement[]){JPE_COLUMN,JPE_OBJ},2,JPE_KEY}, // key: "..":
    },
    .numRules = 1
};
const JExpression jexpJval={
    .checkc = '"',
    .exprRules = (JExprRule[]){
        {(JParseElement[]){JPE_OBJ,JPE_KEY},2,JPE_VAL}, // jval:  "..":".."
    },
    .numRules = 1
};
const JExpression jexpJvals={
    .checkc = ',',
    .exprRules = (JExprRule[]){ 
        // jvals: "..":"..",   |  "..":"..","..":".." | "..":"..","..":"..",
        {(JParseElement[]){JPE_SEPERATE,JPE_VAL},2,JPE_VALS},
        {(JParseElement[]){JPE_VAL,JPE_VALS},2,JPE_MULTIVALS},
        {(JParseElement[]){JPE_VALS,JPE_VALS},2,JPE_MULTIVALSS},
    },
    .numRules = 3
};
const JExpression jexpJson={
    .checkc = '}',
    .exprRules = (JExprRule[]){ 
        {(JParseElement[]){JPE_CLOSE,JPE_VAL,JPE_OPEN},3,JPE_JSON},
        {(JParseElement[]){JPE_CLOSE,JPE_VALS,JPE_OPEN},3,JPE_JSON},
        {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALS,JPE_OPEN},3,JPE_JSON},
        {(JParseElement[]){JPE_CLOSE,JPE_MULTIVALSS,JPE_OPEN},3,JPE_JSON},
    },
    .numRules = 4
};
const JExpression jparse_expressions[]={
    jexpObj,
    jexpKey,
    jexpJval,
    jexpJvals,
    jexpJson
};