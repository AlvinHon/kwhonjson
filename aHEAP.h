#ifndef _H_AHEAP
#define _H_AHEAP


#define ANode struct t_ANode
struct t_ANode{
    ANode* left;
    ANode* right;
    ANode* parent;
    void* key;
    void* content;    
};

#define AStackion struct t_Stackion
struct t_Stackion{
    AStackion* next;
    AStackion* prev;
    void* content;
};

typedef struct{
    AStackion* root;
    AStackion* tail;
}AStack;

ANode* MakeANode(void* key, void* content);
AStack* MakeAStack();
void FreeAStack(AStack* *stack, void (*cxtfree)(void**));
void PushAStack(AStack* *stack, void* content);
int PopAStack(AStack* *stack, void* *content);

#endif