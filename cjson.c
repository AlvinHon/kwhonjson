#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tJSON.h"
#include "jPath.h"
#include "aHEAP.h"
#include "jParseExpr.h"
#include "jParser.h"

void testJSON(void){
    JsonObject* content = MakeJsonObject();
    JsonSet(&content,"name",JSTRING,JsonString("alvin"));
    JsonSet(&content,"id",JSTRING,JsonString("111"));
    JsonSet(&content,"age",JINTEGER,JsonInteger(25));
    JsonObject* info = MakeJsonObject();
    JsonSet(&info,"mobile",JSTRING, JsonString("123456"));
    JsonSet(&content,"info",JSONOBJ,info);
    JsonArray* jarr = MakeJArray();
    JsonAdd(&jarr,"food",JSTRING,JsonString("apple"));
    JsonAdd(&jarr,"drink",JSTRING,JsonString("water"));
    JsonSet(&content,"records",JSONARR,jarr);
    
    printf("start print..\n");
    JsonObjectPrint(content);
    printf("getting ... \n");
    
    JString* namej = JsonGetString(&content,"info/mobile");
    if(namej != NULL)
        printf("get %.*s\n",namej->len,namej->str);
    
    
    FreeJsonObject(&content);
    printf("free json\n");
}

void testHeap(void){
    AStack* gstack = MakeAStack();
    PushAStack(&gstack,"abc");
    PushAStack(&gstack,"abcdef");
    PushAStack(&gstack,"123");
    PushAStack(&gstack,"ppp");
    
    char* popc;
    if(PopAStack(&gstack,(void**)&popc)){
        printf("pop %s\n",popc);
    }
    AStackion* cur = gstack->root;
    while(cur != NULL){
        printf("%s\n",cur->content);
        cur = cur->next;
    }
    
    FreeAStack(&gstack,NULL);
    printf("done\n");
}

int main(int argc, char*argv[]){
    Jobj job;
	printf("CJSON %d!!\n",argc);
	for(int i = 0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
    
    
    const char checking[] = "{\"abc\":\"123\"}";
    JsonParse(checking, sizeof(checking));
	return 0;
}
