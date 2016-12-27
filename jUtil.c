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