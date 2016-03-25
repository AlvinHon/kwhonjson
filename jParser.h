#ifndef _H_JPARSER
#define _H_JPARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aHEAP.h"
#include "tJSON.h"
#include "jParseExpr.h"

#define JPARSER_MALLOC  malloc
#define JPARSER_FREE    free


typedef struct{
    char c;
    JParseElement group;
    JParseElement prev;
}JPElement;

#endif