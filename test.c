#include "imalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct tree *Tree;
typedef struct tree  {
  int current;
  Tree l;
  Tree r;  
} tree;
int main() {
    Manual manMem = (Manual) iMalloc(10 Kb, MANUAL + ADDRESS);
    printf("%d\n", manMem->avail((Memory) manMem));
    char *point = manMem->alloc((Memory) manMem, sizeof(char*)*4);
    printf("%d\n", manMem->avail((Memory) manMem));
    strcpy(point, "hej\0");
    printf("%s\n", point);
    char *point2 = manMem->alloc((Memory) manMem, sizeof(void*));
    printf("%d\n", manMem->avail((Memory) manMem));
    strcpy(point2, "hejsan\0");
    printf("%s\n", point2);
    manMem->free((Memory) manMem, point);
    printf("%d\n", manMem->avail((Memory) manMem)); 
    manMem->free((Memory) manMem, point2);
    printf("%d\n", manMem->avail((Memory) manMem));
 Managed mgrMem = (Managed) iMalloc(10 Kb, GCD + REFCOUNT + ADDRESS);
 Tree t = mgrMem->alloc((Memory) mgrMem, sizeof(tree));
 Tree t1 = mgrMem->alloc((Memory) mgrMem, sizeof(tree));
 Tree t2 = mgrMem->alloc((Memory) mgrMem, sizeof(tree));
 Tree t3 = mgrMem->alloc((Memory) mgrMem, sizeof(tree));
 Tree t4 = mgrMem->alloc((Memory) mgrMem, sizeof(tree));
 
    t->current = 5;
    t1->current = 3;
    t2->current = 6;
    t->l = t1;
    t->r = t2;
    t2->l = t4;
    t4 = NULL;
    t3 = NULL;
    mgrMem->gc.collect((Memory) mgrMem);
    











  
    return 0;
    }
