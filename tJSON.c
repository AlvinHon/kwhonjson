#include "tJSON.h"
#include "jPath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


JsonObject* MakeJsonObject(){
    JsonObject* ret;
    ret = (JsonObject*) malloc(sizeof(JsonObject));
    ret->objects = NULL;
    ret->len = 0;
    ret->capacity = 0;
    ret->root = 1;
    return ret;
}

JsonObject* ExpanseJsonCap(JsonObject* *object){
    JsonObject* ret = MakeJsonObject();
    if(ret != NULL){
        int cap = (*object)->capacity;
        cap = cap == 0 ? 1 : cap;
        ret->len = (*object)->len;
        ret->capacity = cap*2;
        ret->objects = malloc(sizeof(Jobj*) * (ret->capacity));
                //calloc((ret->capacity),sizeof(Jobj*));
        for(int i = 0;i<(ret->len);i++){
            ret->objects[i] = (*object)->objects[i];
        }
        for(int i = (ret->len);i<(ret->capacity);i++){
            ret->objects[i] = NULL;
        }
        free((*object)->objects);
        free(*object);
    }
    return ret;
}

JString* JsonString(const char* name){
    JString* ret;
    ret = (JString*) malloc(sizeof(JString));
    ret->len=strlen(name);
    ret->str = (char*) malloc(ret->len);
    memcpy(ret->str,name,ret->len);
    return ret;
}

int* JsonInteger(int i){
    int* ret = malloc(sizeof(int));
    *ret = i;
    return ret;
}

char* JsonBool(char torf){
    char* ret = malloc(sizeof(char));
    *ret = torf == 't'? 't':'f';
    return ret;
}

void FreeJString(JString** jstr){
    (*jstr)->len = 0;
    free(*jstr);
}

Jobj* MakeJobj(const char* key, JValType type, void* content){
    Jobj* ret = malloc(sizeof(Jobj));
    ret->content=content;
    ret->type=content?type:JNULL;
    ret->key = JsonString(key);
    return ret;
}

JsonArray* MakeJArray(){
    JsonArray* ret;
    ret = malloc(sizeof(JsonArray));
    ret->len = 0;
    ret->array = NULL;
    return ret;
}

void FreeJobj(Jobj* *obj){
    if(*obj != NULL ){
        if((*obj)->type == JSONOBJ){
            FreeJsonObject((JsonObject**)&((*obj)->content));
        }else if ((*obj)->type == JSTRING){
            JString* str = (JString*) ((*obj)->content);
            FreeJString(&str);
        }else if ((*obj)->type == JINTEGER){
            int* intv = (int*) ((*obj)->content);
            free(intv);
        }else if((*obj)->type == JNULL){
            // nothing release
        }else if((*obj)->type == JBOOL){
            char* torf = (char*) ((*obj)->content);
            free(torf);
        }else if ((*obj)->type == JSONARR){
            JsonArray* jarr = (JsonArray*)((*obj)->content);
            JLinkedObj* jcur = jarr->array;
            JLinkedObj* precur;
            while(jcur != NULL){
                 FreeJobj(&(jcur->content));
                 precur = jcur;
                 jcur = jcur->next;
                 free(precur);
            }
        }
        FreeJString(&((*obj)->key));
        free(*obj);
    }
    
}

void FreeJsonObject(JsonObject* *object){
    for (int i = 0;i<((*object)->len);i++){
        FreeJobj(&((*object)->objects[i]));
    }
    free((*object)->objects);
    free(*object);
}

void JsonPrint(const Jobj* obj){
    if(obj != NULL){
        // print key first for json object
        if(obj->type == JSONOBJ || obj->type==JSTRING || obj->type == JSONARR
            || obj->type == JINTEGER)
            printf("\"%.*s\":",obj->key->len,obj->key->str);
        
        if(obj->type == JSONOBJ){
            printf("{");
            JsonObjectPrint((const JsonObject*)(obj->content));
            printf("}");
        }else if (obj->type == JINTEGER){
            int* intv = (int*) (obj->content);
            printf("%d",*intv);
        }else if (obj->type == JNULL){
            printf("null");
        }else if (obj->type == JBOOL){
            char* torf = (char*)(obj->content);
            printf("%s",(*torf) == 't' ? "true":"false");
        }else if(obj->type==JSTRING  && obj->content != NULL){
            JString* str = (JString*) (obj->content);
            printf("\"%.*s\"",str->len,(const char*)(str->str));
        }else if (obj->type == JSONARR){
            printf("[");
            JsonArray* jarr = (JsonArray*) (obj->content);
            JLinkedObj* jcur = jarr->array;
            if(jcur != NULL){
                JsonPrint(jcur->content); // first item
                if(jcur->next != NULL)
                    printf(",");
                
                while(jcur->next != NULL){
                    jcur = jcur->next;
                    JsonPrint(jcur->content);
                    if(jcur->next != NULL)
                        printf(",");
                }
                
            }
            printf("]");
        }
    }
}

void JsonObjectPrint(const JsonObject* object){
    if(object->root)
        printf("{");
    for(int i = 0;i<object->len;i++){
        JsonPrint(object->objects[i]); 
        if( i != (object->len -1))
            printf(",");
    }
    if(object->root)
        printf("}\n");
}


JsonObject* JsonSet(JsonObject* *object, const char* key, JValType type, void* val){
    int nlen = (*object)->len + 1;
    if(nlen >= (*object)->capacity){ // overflow
        (*object) = ExpanseJsonCap(object);
    }
    if((*object) != NULL){
        if(type == JSONOBJ){
            ((JsonObject*)(val))->root = 0;
        }
        (*object)->objects[(*object)->len] = MakeJobj(key,type,val);
        (*object)->len = nlen;
    }
    return *object;
}



Jobj* JsonGet(JsonObject* *object,const char* path){
    void* ret = NULL;
    PathStr* xpath = MakePath('/',path,strlen(path));
    PathStr* xlevel = xpath;
    JsonObject* olevel = *object;
    while(xlevel != NULL){
        for(int i = 0;i<olevel->len;i++){
            Jobj* searchobj = olevel->objects[i];
            if(strcmp(searchobj->key->str,xlevel->path)==0){
                if(xlevel->next != NULL && searchobj->type == JSONOBJ){
                    // search next level
                    olevel = (JsonObject*) searchobj->content;
                }else if (xlevel->next == NULL){
                    // terminating
                    ret = searchobj;
                }else{
                    // TODO not terminating and this is not json object
                }
                break;
            }
        }
        
        xlevel = xlevel->next;
    }
    FreePath(&xpath);
    return ret;
}

JString* JsonGetString(JsonObject* *object, const char* path){
    JString* ret = NULL;
    Jobj* obj = JsonGet(object,path);
    if(obj != NULL && obj->type == JSTRING){
        return (JString*)(obj->content);
    }
    return ret;
}
int* JsonGetInt(JsonObject* *object, const char* path){
    int* ret = NULL;
    Jobj* obj = JsonGet(object,path);
    if(obj != NULL && obj->type == JINTEGER){
        return (int*)(obj->content);
    }
    return ret;
}
char* JsonGetBool(JsonObject* *object, const char* path){
    char* ret = NULL;
    Jobj* obj = JsonGet(object,path);
    if(obj != NULL && obj->type == JBOOL){
        return (char*)(obj->content);
    }
    return ret;
}

void JsonAdd(JsonArray* *jarray, const char* key, JValType type, void* val){
    JLinkedObj* cur = (*jarray)->array;
    JLinkedObj* pcur = cur;
    while(cur != NULL){
        pcur = cur;
        cur = cur->next;
    }
    
    if(pcur == NULL){ // first
        (*jarray)->array = malloc(sizeof(JLinkedObj));
        (*jarray)->array->next = NULL;
        (*jarray)->array->content = MakeJobj(key,type,val);
    }else {
        pcur->next = malloc(sizeof(JLinkedObj));
        pcur->next->next = NULL;
        pcur->next->content = MakeJobj(key,type,val);
    }
}