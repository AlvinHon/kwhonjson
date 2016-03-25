
#include "jParser.h"


JPElement* MakeJPElement(char c, JParseElement g){
   JPElement* ret = JPARSER_MALLOC(sizeof(JPElement));
   ret->c = c;
   ret->group = g;
   ret->prev = JPE_NULL;
   return ret;
}

JParseElement EscapeCharElement(char c){
    switch(c){
        case '{': return JPE_OPEN;
        case '}': return JPE_CLOSE;
        case '"': return JPE_STRTER;
        case ',': return JPE_SEPERATE;
        case ':': return JPE_COLUMN;
        default: break;
    }
    return JPE_NULL;
}

// msg: preparsed " -> \"
int Parser(const char* msg, size_t len){
    AStack* frameStack = MakeAStack();
    if(frameStack == NULL) return 0;
    AStack* contextStack = MakeAStack();
    if(contextStack == NULL) return 0;
    
    
    for(size_t i = 0;i<len;i++){
        // do escape here
        if(msg[i] == '\\' && (i!= len-1)){
            if(EscapeCharElement(msg[i+1]) != JPE_NULL)
                continue; // escape here
        }
        
        // push pop char
        JParseElement element = EscapeCharElement(msg[i]);
        if(element == JPE_NULL){
            JPElement* op = MakeJPElement(msg[i],JPE_ANY);
            PushAStack(&frameStack,op);
            continue;
        }
        
            //JPElement* op = MakeJPElement(msg[i],)
            //*op = JPE_OPEN;
            //PushAStack(&frameStack,op);
        
    }
    
    
    return 1;
}
