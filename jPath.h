#ifndef _H_JPATH
#define _H_JPATH

#define PathStr struct t_pathstr
struct t_pathstr{
    PathStr* next;
    char* path;
};

PathStr* MakePath(char seperator, const char* path, int len);
void FreePath(PathStr* *path);

#endif