#include "jUtil.h"


int IsStrNumber(const char* str){
    size_t strl = strlen(str);
    int i;
    if(strl == 0)
        return 0;
    for(i=0;i<strl;i++){
        if(!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int IsStrNull(const char* str){
    if(str == NULL)
        return 1;
    return (memcmp(str,"null",4) == 0) ? 1 : 0;
}

int IsStrBool(const char* str){
    if(str != NULL){
        if(memcmp(str,"True",4) == 0 ||
            memcmp(str,"true",4) == 0 ||
            memcmp(str,"False",5) == 0 ||
            memcmp(str,"false",5) == 0 
            ){
                return 1;
        }
    }
    return 0;
}