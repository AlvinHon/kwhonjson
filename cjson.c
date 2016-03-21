#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tJSON.h"
#include "jPath.h"


int main(int argc, char*argv[]){
    Jobj job;
	printf("CJSON %d!!\n",argc);
	for(int i = 0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
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
	return 0;
}
