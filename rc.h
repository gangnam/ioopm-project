#ifndef __rc_h
#define __rc_h
#include "rc.c"
unsigned int increaseReferenceCounter (Chunk ptr);

unsigned int decreaseReferenceCounter (Memory mem, Chunk ptr);

unsigned int returnReferenceCounter (Chunk ptr);
#endif 