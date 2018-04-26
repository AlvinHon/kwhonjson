#include "tJSON.h"
#include "jPath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TJSON_MALLOC    malloc
#define TJSON_FREE      free

static size_t StrAppend(char* *buf, size_t bufsize, const char* str, size_t strl ){
    char* tmpbuf;

    if(strl == 0 || str == NULL){
        return bufsize;
    }
    tmpbuf = TJSON_MALLOC(bufsize + strl);

    if(bufsize > 0){
        memcpy(tmpbuf,*buf,bufsize);
    }
    memcpy(tmpbuf + bufsize,str,strl);

    if(*buf){
        TJSON_FREE(*buf);
    }
    *buf = tmpbuf;
    tmpbuf = NULL;
    return bufsize + strl;
}

JsonObject* MakeJsonObject(){
    JsonObject* ret;
    ret = (JsonObject*) TJSON_MALLOC(sizeof(JsonObject));
    ret->objects = NULL;
    ret->len = 0;
    ret->capacity = 0;
    ret->root = NULL;
    return ret;
}

JsonObject* ExpanseJsonCap(JsonObject* *object){
    int len = (*object)->len;
    int cap = (*object)->capacity;
    cap = cap == 0 ? 1 : cap*2;
    Jobj** newArr = TJSON_MALLOC(sizeof(Jobj*) * (cap));
    if(newArr == NULL)
        return NULL;
    for(int i = 0;i<len;i++){
        newArr[i] = (*object)->objects[i];
    }
    for(int i =len;i<cap;i++){
        newArr[i] = NULL;
    }
    if((*object)->objects != NULL){
        TJSON_FREE((*object)->objects);
    }
    (*object)->objects = newArr;
    (*object)->len = len;
    (*object)->capacity = cap;
    return *object;
}

JString* JsonString(const char* name){
    JString* ret;
    ret = (JString*) TJSON_MALLOC(sizeof(JString));
    if(name != NULL){
        ret->len=strlen(name);
        ret->str = (char*) TJSON_MALLOC(ret->len);
        memcpy(ret->str,name,ret->len);
    }else{
        ret->len=0;
        ret->str = NULL;
    }
    return ret;
}

void FreeJString(JString** jstr) {
	(*jstr)->len = 0;
	if ((*jstr)->str) {
		TJSON_FREE((*jstr)->str);
		(*jstr)->str = NULL;
	}
	TJSON_FREE(*jstr);
}

void EditJString(JString** jstr, const char* newcontent) {
	if ((*jstr)->str) {
		TJSON_FREE((*jstr)->str);
		(*jstr)->str = NULL;
	}
	(*jstr)->len = strlen(newcontent);
	(*jstr)->str = (char*)TJSON_MALLOC((*jstr)->len);
	memcpy((*jstr)->str, newcontent, (*jstr)->len);
}

int* JsonInteger(int i){
    int* ret = TJSON_MALLOC(sizeof(int));
    *ret = i;
    return ret;
}

char* JsonBool(char torf) {
	char* ret = TJSON_MALLOC(sizeof(char));
	*ret = torf == 't' ? 't' : 'f';
	return ret;
}

Jobj* MakeJobj(const char* key, JValType type, void* content){
    Jobj* ret = TJSON_MALLOC(sizeof(Jobj));
    ret->content=content;
    ret->type=content?type:JNULL;
    ret->key = JsonString(key);
    return ret;
}

void EditJobj(Jobj** jobj, JValType newtype, void* newi) {
	if ((*jobj)->type == JINTEGER && newtype == JINTEGER) {
		if ((*jobj)->content != NULL) {
			TJSON_FREE((*jobj)->content);
			(*jobj)->content = JsonInteger(*(int*)(newi));
		}
	}
	else if ((*jobj)->type == JBOOL && newtype == JBOOL) {
		if ((*jobj)->content != NULL) {
			TJSON_FREE((*jobj)->content);
			(*jobj)->content = JsonBool(*(char*)(newi));
		}
	}
}

JsonArray* MakeJArray(){
    JsonArray* ret;
    ret = TJSON_MALLOC(sizeof(JsonArray));
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
            TJSON_FREE(intv);
        }else if((*obj)->type == JNULL){
            // nothing release
        }else if((*obj)->type == JBOOL){
            char* torf = (char*) ((*obj)->content);
            TJSON_FREE(torf);
        }else if ((*obj)->type == JSONARR){
            JsonArray* jarr = (JsonArray*)((*obj)->content);
            JLinkedObj* jcur = jarr->array;
            JLinkedObj* precur;
            while(jcur != NULL){
                 FreeJobj(&(jcur->content));
                 precur = jcur;
                 jcur = jcur->next;
                 TJSON_FREE(precur);
            }
        }
        FreeJString(&((*obj)->key));
        TJSON_FREE(*obj);
    }
    
}

void FreeJsonObject(JsonObject* *object){
    for (int i = 0;i<((*object)->len);i++){
        FreeJobj(&((*object)->objects[i]));
    }
    TJSON_FREE((*object)->objects);
    TJSON_FREE(*object);
}

void JsonFPrint(const Jobj* obj, FILE* f){
    if(obj != NULL){
        // print key first for json object
        if(obj->type == JSONOBJ || obj->type==JSTRING || obj->type == JSONARR
            || obj->type == JINTEGER || obj->type == JBOOL || obj->type == JNULL )
            fprintf(f,"\"%.*s\":",obj->key->len,obj->key->str);
        
        if(obj->type == JSONOBJ){
            fprintf(f,"{");
            JsonObjectFPrint((const JsonObject*)(obj->content), f);
            fprintf(f,"}");
        }else if (obj->type == JINTEGER || obj->type == PLAIN_INTEGER){
            int* intv = (int*) (obj->content);
            fprintf(f,"%d",*intv);
        }else if (obj->type == JNULL){
            fprintf(f,"null");
        }else if (obj->type == JBOOL || obj->type == PLAIN_BOOL){
            char* torf = (char*)(obj->content);
            fprintf(f,"%s",(*torf) == 't' ? "true":"false");
        }else if((obj->type==JSTRING || obj->type == PLAIN_STRING) && obj->content != NULL){
            JString* str = (JString*) (obj->content);
            fprintf(f,"\"%.*s\"",str->len,(const char*)(str->str));
        }else if (obj->type == JSONARR){
            fprintf(f,"[");
            JsonArray* jarr = (JsonArray*) (obj->content);
            JLinkedObj* jcur = jarr->array;
            if(jcur != NULL){
                JsonFPrint(jcur->content,f); // first item
                if(jcur->next != NULL)
                    fprintf(f,",");
                
                while(jcur->next != NULL){
                    jcur = jcur->next;
					JsonFPrint(jcur->content, f);
                    if(jcur->next != NULL)
                        fprintf(f,",");
                }
                
            }
            fprintf(f,"]");
        }
    }
}
void JsonPrint(const Jobj* obj){
    JsonFPrint(obj,stdout);
}

void JsonObjectFPrint(const JsonObject* object, FILE* f){
     if(object->root == NULL)
        fprintf(f,"{");
    for(int i = 0;i<object->len;i++){
        JsonPrint(object->objects[i]); 
        if( i != (object->len -1))
            fprintf(f,",");
    }
    if(object->root == NULL)
      fprintf(f,"}\n");
}
void JsonObjectPrint(const JsonObject* object){
   JsonObjectFPrint(object,stdout);
}

JsonObject* JsonSet(JsonObject* *object, const char* key, JValType type, void* val){
    int nlen = (*object)->len + 1;
    if(nlen >= (*object)->capacity){ // overflow
        (*object) = ExpanseJsonCap(object);
    }
    if((*object) != NULL){
        if(type == JSONOBJ){
            ((JsonObject*)(val))->root = *object;
            ((JsonObject*)(val))->rootType = JSONOBJ;
        }else if(type==JSONARR){
            ((JsonArray*)(val))->root = *object;
            ((JsonArray*)(val))->rootType = JSONOBJ;
        }
        (*object)->objects[(*object)->len] = MakeJobj(key,type,val);
        (*object)->len = nlen;
    }
    return *object;
}



JSearchable JsonGet(JsonObject* *object,const char* path){
    PathStr* xpath = MakePath('/',path,strlen(path));
    PathStr* xlevel = xpath;
    JsonObject* olevel = *object;
    JsonArray* olevelarr = NULL;
    JValType oleveltype = JSONOBJ;
    
    JSearchable retSearch;
    retSearch.kind = -1;

    while(xlevel != NULL){
        // currently at json object
        if(oleveltype == JSONOBJ){
            for(int i = 0;i<olevel->len;i++){
                Jobj* searchobj = olevel->objects[i];
                //printf("level: %d %.*s vs %s\n",i,searchobj->key->len,searchobj->key->str,xlevel->path);
                if(strlen(xlevel->path) == searchobj->key->len && strncmp(searchobj->key->str,xlevel->path, searchobj->key->len)==0){
                    //printf("hit!\n");
                    if(xlevel->next != NULL){
                        // search next level
                        //printf("next level %d !\n",searchobj->type);
                        if(searchobj->type == JSONOBJ){
                            olevel = (JsonObject*) searchobj->content;
                            oleveltype = JSONOBJ;
                        }else if(searchobj->type == JSONARR){
                            olevelarr = (JsonArray*) searchobj->content;
                            oleveltype = JSONARR;
                        }
                    }else if (xlevel->next == NULL){
                        // terminating
                        //printf("terminating %d!\n", searchobj->type);
                        if (searchobj->type == JSONARR){
                            retSearch.thing.array = ((JsonArray*) searchobj->content)->array;;
                            retSearch.kind = 1;
						} else {
							retSearch.thing.obj = searchobj;
							retSearch.kind = 0;
						}
                    }else{
                        // TODO not terminating and this is not json object
                    }
                    break;
                }
            }
        }else{
            // currently at array object
            JLinkedObj* cursor = olevelarr->array;
            while(cursor != NULL){
                Jobj* searchobj = cursor->content;
                //printf("cursor != NULL\n");
                if(searchobj->key == NULL && xlevel->next == NULL){
                    // this array has plain raw value
                    // olevelarr is the expected return value
                    retSearch.thing.array = cursor;
                    retSearch.kind = 1;
                    printf("plain raw values\n");
                    break;
                }
                if(strcmp(searchobj->key->str,xlevel->path) == 0){
                    if(xlevel->next != NULL){
                        // search next level
                        //printf("array next level!\n");
                        if(searchobj->type == JSONOBJ){
                            olevel = (JsonObject*) searchobj->content;
                            oleveltype = JSONOBJ;
                        }else if(searchobj->type == JSONARR){
                            olevelarr = (JsonArray*) searchobj->content;
                            oleveltype = JSONARR;
                        }
                    }else if (xlevel->next == NULL){
                        // terminating
                        //printf("array terminating!\n");
                        retSearch.thing.obj = searchobj;
                        retSearch.kind = 0;
                    }
                    break;
                }
                cursor = cursor->next;
            }
        }
        
        xlevel = xlevel->next;
    }
    FreePath(&xpath);
    return retSearch;
}

int ConvertToJString(JString* forceStr, Jobj* obj){
    char intbuf[256];
    memset(intbuf,0x0,sizeof(intbuf));

    if(forceStr != NULL && obj != NULL){
        // clear forceStr
        if(forceStr->str){
            TJSON_FREE(forceStr->str);
            forceStr->str = NULL;
        }
        forceStr->len = 0;

        if(obj->type == JINTEGER){
            sprintf_s(intbuf, sizeof(intbuf), "%d",*((int*)obj->content));    
        }else if(obj->type == JSTRING && obj->content != NULL){
            JString* str = (JString*) (obj->content);
            sprintf_s(intbuf, sizeof(intbuf), "\"%.*s\"",str->len,(const char*)(str->str));
        }else if(obj->type == JBOOL){
            if(*((char*)obj->content) == 't'){
                sprintf_s(intbuf,sizeof(intbuf),"true");
            }else{
                sprintf_s(intbuf,sizeof(intbuf),"false");
            }
        }else if(obj->type == JNULL){
            sprintf_s(intbuf,sizeof(intbuf),"null");
        }
        
        forceStr->len = strlen(intbuf);
        if(forceStr->len > 0){
            forceStr->str = TJSON_MALLOC(forceStr->len );
            memcpy(forceStr->str,intbuf,forceStr->len);
        }
    }
    return 1;
}

int ConvertArrayToJString(JString* forceStr, JLinkedObj* cursor){
    char sbuf[256];
    char * buf;
    size_t buflen = 1;

    if(forceStr != NULL && cursor != NULL){
        // clear forceStr
        if(forceStr->str){
            TJSON_FREE(forceStr->str);
            forceStr->str = NULL;
        }
        forceStr->len = 0;


        buf = TJSON_MALLOC(buflen);
        buf[0] = '[';

        while(cursor != NULL){
            Jobj* obj = cursor->content;
            if(obj != NULL){
				//printf("cursor type=%d\n", obj->type);
                if(obj->type == PLAIN_STRING){
                    JString* jstr = (JString*)obj->content;
                    buflen = StrAppend(&buf,buflen,jstr->str,jstr->len);
                }else if(obj->type == PLAIN_INTEGER){
                    memset(sbuf,0x0, sizeof(sbuf));
                    sprintf_s(sbuf, sizeof(sbuf), "%d",*((int*)obj->content));
                    buflen = StrAppend(&buf,buflen,sbuf,strlen(sbuf));
				}
				else if (obj->type == PLAIN_BOOL) {
					if (*(char*)(obj->content) == 't') {
						buflen = StrAppend(&buf, buflen, "true", 4);
					}
					else {
						buflen = StrAppend(&buf, buflen, "false", 4);
					}
				}
				else if (obj->type == JINTEGER) {
					memset(sbuf, 0x0, sizeof(sbuf));
					sprintf_s(sbuf, sizeof(sbuf), "%.*s:%d", obj->key->len, obj->key->str, *((int*)obj->content));
					buflen = StrAppend(&buf, buflen, sbuf, strlen(sbuf));
				}
				else if (obj->type == JSTRING) {
					memset(sbuf, 0x0, sizeof(sbuf));
					JString* str = (JString*)(obj->content);
					sprintf_s(sbuf, sizeof(sbuf), "%.*s:%.*s", obj->key->len, obj->key->str, str->len, str->str);
					buflen = StrAppend(&buf, buflen, sbuf, strlen(sbuf));
				}
				else if (obj->type == JBOOL) {
					memset(sbuf, 0x0, sizeof(sbuf));
					if(*(char*)(obj->content) == 't')
						sprintf_s(sbuf, sizeof(sbuf), "%.*s:true", obj->key->len, obj->key->str);
					else
						sprintf_s(sbuf, sizeof(sbuf), "%.*s:false", obj->key->len, obj->key->str);
					buflen = StrAppend(&buf, buflen, sbuf, strlen(sbuf));
				}
            }
            cursor = cursor->next;
            if(cursor != NULL){
                buflen = StrAppend(&buf,buflen,",",1);
            }
        }
        buflen = StrAppend(&buf,buflen,"]",1);
        forceStr->str = buf;
        forceStr->len = buflen;
        buf = NULL;
    }

    return 1;
}

/*
forceStr: NULL if not require forcing output to JString object
*/
JString* JsonGetString(JsonObject** object, const char* path, JString* forceStr){
    JSearchable retSearch = JsonGet(object,path);
	
    if(retSearch.kind == 0){
        Jobj* obj = retSearch.thing.obj;
        if(obj != NULL){
            if(forceStr != NULL){
                ConvertToJString(forceStr,obj);
                return forceStr;
            }else if(obj->type  == JSTRING){
                return (JString*)(obj->content);
            }
        }
    }else if (retSearch.kind == 1 && forceStr != NULL){
        // turning array to JString
        ConvertArrayToJString(forceStr,retSearch.thing.array);
        return forceStr;
    }
	
    return NULL;
}

int JsonEditJString(JsonObject** object, const char* path, const char* newcontent) {
	JSearchable retSearch = JsonGet(object, path);

	Jobj* obj = retSearch.thing.obj;
	if (retSearch.kind == 0 && obj != NULL) {
		if (obj->type == JSTRING && obj->content != NULL) {
			JString* str = (JString*)(obj->content);
			EditJString(&str, newcontent);
			return 1;
		}
	}
	return 0;
}

int JsonEditJInteger(JsonObject** object, const char* path, int val) {
	JSearchable retSearch = JsonGet(object, path);

	Jobj* obj = retSearch.thing.obj;
	if (retSearch.kind == 0 && obj != NULL) {
		if (obj->type == JINTEGER && obj->content != NULL) {
			EditJobj(&obj, JINTEGER, &val);
			return 1;
		}
	}
	return 0;
}

int JsonEditJBool(JsonObject** object, const char* path, int val) {
	char tf = val == 1 ? 't' : 'f';
	JSearchable retSearch = JsonGet(object, path);

	Jobj* obj = retSearch.thing.obj;
	if (retSearch.kind == 0 && obj != NULL) {
		if (obj->type == JBOOL && obj->content != NULL) {
			EditJobj(&obj, JBOOL, &tf);
			return 1;
		}
	}
	return 0;
}

void JsonAdd(JsonArray* *jarray, const char* key, JValType type, void* val){
    JLinkedObj* cur = (*jarray)->array;
    JLinkedObj* pcur = cur;
    while(cur != NULL){
        pcur = cur;
        cur = cur->next;
    }
    
    if(pcur == NULL){ // first
        (*jarray)->array = TJSON_MALLOC(sizeof(JLinkedObj));
        (*jarray)->array->next = NULL;
        (*jarray)->array->content = MakeJobj(key,type,val);
        
    }else {
        pcur->next = TJSON_MALLOC(sizeof(JLinkedObj));
        pcur->next->next = NULL;
        pcur->next->content = MakeJobj(key,type,val);
    }

    if(type == JSONOBJ){
        ((JsonObject*)val)->root = *jarray;
        ((JsonObject*)val)->rootType = JSONARR;
    }else if(type==JSONARR){
        ((JsonArray*)val)->root = *jarray;
        ((JsonArray*)val)->rootType = JSONARR;
    }

}

void JsonAddRawValue(JsonArray* *jarray, JValType type, void* val){
    if(type == PLAIN_STRING || type == PLAIN_INTEGER || type == PLAIN_BOOL){
        JsonAdd(jarray,NULL,type,val);
    }
}