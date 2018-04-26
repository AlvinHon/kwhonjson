#ifndef _TJSON_H
#define _TJSON_H

#include <stdio.h>
#include <stddef.h>

typedef enum {
    JSONOBJ,
    JSONARR,
    JSTRING,
    JINTEGER,
    JBOOL,
    JNULL,
    PLAIN_STRING,
    PLAIN_INTEGER,
    PLAIN_BOOL
}JValType;

typedef struct{
    char* str;
    int len;
}JString;

typedef struct{
    JString* key;
    void* content;
    JValType type;
}Jobj;

#define JLinkedObj struct t_JLinkedObj
struct t_JLinkedObj{
    JLinkedObj* next;
    Jobj* content;
};

typedef struct{
    union{
        Jobj* obj;
        JLinkedObj* array;
    }thing;
    int kind;   // 0: obj, 1: array
}JSearchable;

typedef struct{
    JLinkedObj* array;
    int len;
    void* root;
    JValType rootType;
}JsonArray;

typedef struct{
    Jobj** objects;
    int len;
    int capacity;
    void* root;
    JValType rootType;
}JsonObject;


// outer
JsonObject* MakeJsonObject();
JsonArray* MakeJArray();

JString* JsonString(const char* name);
int* JsonInteger(int i);
char* JsonBool(char torf);
void FreeJString(JString** jstr);

JsonObject* JsonSet(JsonObject* *object, const char* key, JValType type, void* val);
void JsonAdd(JsonArray* *array, const char* key, JValType type, void* val);
void JsonAddRawValue(JsonArray* *jarray, JValType type, void* val);

JSearchable JsonGet(JsonObject* *object,const char* path);
JString* JsonGetString(JsonObject** object, const char* path, JString* forceStr);
int JsonEditJString(JsonObject** object, const char* path, const char* newcontent);
int JsonEditJInteger(JsonObject** object, const char* path, int val);
int JsonEditJBool(JsonObject** object, const char* path, int val);

void FreeJsonObject(JsonObject* *object);
void JsonObjectFPrint(const JsonObject* onject, FILE* f);
void JsonObjectPrint(const JsonObject* object);

// inner
void JsonFPrint(const Jobj* obj, FILE* f);
void JsonPrint(const Jobj* obj);
Jobj* MakeJobj(const char* key, JValType type, void* content);

#endif