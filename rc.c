#include <stdio.h>
#include <stdlib.h>
#include "priv_imalloc.h"

unsigned int increaseReferenceCounter (void *ptr) {
    Chunk temp = (Chunk) (ptr-sizeof(chunk));
    temp->refcount++;
    return temp->refcount;
}

unsigned int decreaseReferenceCounter (Memory mem, void *ptr) {
    Chunk temp = (Chunk) (ptr-sizeof(chunk));
    temp->refcount--;
    if (temp->refcount == 0) {
        temp->free = 1;
        //InsertToFreelist(mem, temp);
    }
    return temp->refcount;
}

unsigned int returnReferenceCounter (void *ptr) {
    Chunk temp = (Chunk) (ptr-sizeof(chunk));
    return temp->refcount;
}
