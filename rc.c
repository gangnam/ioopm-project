#ifndef __rc_c
#define __rc_c 
#include <stdio.h>
#include <stdlib.h>



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
    #endif