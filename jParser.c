
#include "jParser.h"


JPElement* MakeJPElement(char c, JParseElement g){
   JPElement* ret = JPARSER_MALLOC(sizeof(JPElement));
   ret->c = c;
   ret->group = g;
   ret->prev = JPE_NULL;
   return ret;
}
void FreeJPElement(void* *content){
    JPARSER_FREE(*((JPElement**)content));
}

JParseElement EscapeCharElement(char c){
    switch(c){
        case '{': return JPE_OPEN;
        case '}': return JPE_CLOSE;
        case '"': return JPE_STRTER;
        case ',': return JPE_SEPERATE;
        case ':': return JPE_COLUMN;
        case '[': return JPE_AOPEN;
        case ']': return JPE_ACLOSE;
        default: break;
    }
    return JPE_NULL;
}

typedef struct{
    AStackion* tail;
    AStackion* terminateAt;
}RuleCheckObject;

int RuleChecker(const JParseElement* jarr,int arrlen,void** stackobj){
    AStackion* tail = ((RuleCheckObject*)(*stackobj))->tail;
    if(tail == NULL || (((JPElement*)(tail->content))->group) != jarr[0])  return 0;

    for(int i = 0;i<arrlen;i++){
        JParseElement jpele = (((JPElement*)(tail->content))->group);
        if(jarr[i] != jpele){
            return 0;
        }
        //skip all any type char
        while(tail->prev != NULL && (((JPElement*)(tail->prev->content))->group) == JPE_ANY) {tail = tail->prev;}
        // not JPE_ANY for next check
        ((RuleCheckObject*)(*stackobj))->terminateAt = tail;
        tail = tail->prev;
        if(tail == NULL){
            return (i == (arrlen-1)) ? 1 : 0;
        }
    }
    return 1;
    
}


// msg: preparsed " -> \" in content
int JsonParse(const char* msg, size_t len,JsonObject** resJson){
    if(resJson == NULL)
      return -1;
    AStack* frameStack = MakeAStack();
    if(frameStack == NULL)
      return -2;
    JPEContainer* jcont = InitContainer(resJson);
    if(!jcont){
      return -3;
    }

    // TODO check also first and last char must be { and }
    JPARSE_TRACE("len: %zu\n",len);
    int escapeflag = 0;
    for(size_t i = 0;i<len;i++){
        // do escape here
        char msgc = msg[i];
        if(msgc == '\\' && (i!= len-1)){
            if(escapeflag == 0){ // first '\\'
                escapeflag = 1;
            }else if (escapeflag == 1){ // two '\\'
                escapeflag = 0;
            }
        }else if (escapeflag == 1){ // '\\' before
            escapeflag = 2;
        }
        
        // push char
        JParseElement element = EscapeCharElement(msgc);
        if(element == JPE_NULL || escapeflag == 2){
            JPARSE_TRACE("\tc-any: %c\n",msgc);
            JPElement* op = MakeJPElement(msgc,JPE_ANY);
            PushAStack(&frameStack,op);
            if(escapeflag == 2){
                escapeflag = 0; // reset
            }
            continue;
        }else{
            JPARSE_TRACE("\tc-elem: %c\n",msgc);
            JPElement* op = MakeJPElement(msgc,element);
            PushAStack(&frameStack,op);
            JPARSE_TRACE("push %s\n",JPENAME[element]);
        }

        
        // check stack
        const JExprRule* jres;
        do{
            RuleCheckObject queryobj;
            RuleCheckObject* ptrQObj;
            ptrQObj = &queryobj;
            ptrQObj->tail = frameStack->tail;
            ptrQObj->terminateAt = NULL;
            jres = JResultFromExp(&RuleChecker,(void**)&(ptrQObj));
            if(jres != NULL){
              JPARSE_TRACE("rule result %s..\n",JPENAME[jres->result]);
              DArray* chrarr = NULL;
              chrarr = MakeDArray(1);

              // some rules having result 
              // pop first and then put the result
              if(jres->ruleMakeJson != NULL && jres->result == JPE_NULL){
                  jres->ruleMakeJson(jcont,NULL);
                  break;
              }

              if(ptrQObj-> terminateAt != NULL){
                while(frameStack->tail != NULL && frameStack->tail != ptrQObj->terminateAt){
                   char popc = (char)(((JPElement*)(frameStack->tail->content))->c);
                   JParseElement popg = ((JPElement*)(frameStack->tail->content))->group;
                   JPARSE_TRACE("pop %s (%c)..\n",JPENAME[popg],popc);

                   if(popg == JPE_ANY)
                      AddDArray(&chrarr,popc);
                   
                   PopAStack(&frameStack,NULL);
                }
              }
              if(frameStack->tail == NULL){ // imposible
                JPARSE_TRACE("impossible tail null\n");
                return 0; // FIXME
              }
              // tail is last element, do last pop
              JPARSE_TRACE("pop %s (%c)..\n",
                JPENAME[((JPElement*)(frameStack->tail->content))->group],
                (char)(((JPElement*)(frameStack->tail->content))->c));
              PopAStack(&frameStack,NULL);

              // push result
              JPARSE_TRACE("push %s\n",JPENAME[jres->result]);
              JPElement* pushelem = MakeJPElement(msgc,jres->result);
              PushAStack(&frameStack,pushelem);

              // make linkage
              ReDArray(&chrarr);
              AddDArray(&chrarr,0x0);

              const char* strinput = NULL;
              if(chrarr != NULL)
                strinput = chrarr->arr;
              
              if(jres->ruleMakeJson != NULL){
                jres->ruleMakeJson(jcont,strinput);
              }
              
              FreeDArray(&chrarr);
            }
        }while(jres != NULL);
    }

    if(LenAStack(frameStack) != 0){
        printf("Warning!! Json Object maybe malformed. Stack Size: %d > 0\n", LenAStack(frameStack));
    }
    
    FreeContainer(&jcont);
    FreeAStack(&frameStack,&FreeJPElement);
    return 1;
}
