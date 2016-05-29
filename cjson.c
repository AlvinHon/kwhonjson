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

void testDArray(void){
    DArray* da = MakeDArray(1);
    char inp[5] = {'a','b','d','e','h'};
    for(int i = 0;i<5;i++){
        printf("add %c %d ",inp[i],AddDArray(&da,inp[i]));
        printf("%zu\n",da->size);
    }
    FreeDArray(&da);
}

void testParseJson(void){
    const char checking[] = "{\"abc\":\"123\",\"def\":\"apple\"}";
    JsonObject* jsonobj = MakeJsonObject();
    JsonParse(checking, strlen(checking),&jsonobj);
    JsonObjectPrint(jsonobj);
    FreeJsonObject(&jsonobj);
}


int main(int argc, char*argv[]){
	printf("CJSON %d!!\n",argc);
	for(int i = 0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
    
    testParseJson();
	return 0;
}
