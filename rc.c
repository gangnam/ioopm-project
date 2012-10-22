#include <stdio.h>
#include <stdlib.h>
#include "imalloc.h"
#include "rc.h"
#include "priv_imalloc.h"


unsigned int increaseReferenceCounter (Chunk ptr) {
    ptr->refcount++;
    return ptr->refcount;
    }

unsigned int decreaseReferenceCounter (Memory mem, Chunk ptr) {
    ptr->refcount--;
    // do some cool stuff
    if (ptr->refcount == 0) {
        ptr->free = 1;

    } 

    return ptr->refcount;
    }

unsigned int returnReferenceCounter (Chunk ptr) {
    return ptr->refcount;
    }