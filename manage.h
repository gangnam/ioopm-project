#ifndef __manage_h
#define __manage_h
#include "priv_imalloc.h"

void GC(Chunk c);
void setZero (Chunk c);
void traverseStack(AdressSpace h, Markfun f, void *p);
void freeObj (chunk c);
void printPtr(void *ptr, void *ignore);

#endif