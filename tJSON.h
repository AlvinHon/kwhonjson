#ifndef _TJSON_H
#define _TJSON_H

typedef enum{
    JSONOBJ,
    JSONARR,
    JSTRING,
    JINTEGER,
    JBOOL,
    JNULL,
    PLAIN_JSTRING
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
    JLinkedObj* array;
    int len;
}JsonArray;

typedef struct{
    Jobj** objects;
    int len;
    int capacity;
    int root;
}JsonObject;

// outer
JsonObject* MakeJsonObject();
JsonArray* MakeJArray();

JString* JsonString(const char* name);
int* JsonInteger(int i);
char* JsonBool(char torf);

JsonObject* JsonSet(JsonObject* *object, const char* key, JValType type, void* val);
void JsonAdd(JsonArray* *array, const char* key, JValType type, void* val);

Jobj* JsonGet(JsonObject* *object,const char* path);
JString* JsonGetString(JsonObject* *object, const char* path);
int* JsonGetInt(JsonObject* *object, const char* path);
char* JsonGetBool(JsonObject* *object, const char* path);

void FreeJsonObject(JsonObject* *object);
void JsonObjectPrint(const JsonObject* object);

// inner
void JsonPrint(const Jobj* obj);
Jobj* MakeJobj(const char* key, JValType type, void* content);

#endif