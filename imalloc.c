#include <stdio.h>
#include <stdlib.h>
#include "imalloc.h"
#include "priv_imalloc.h"
#include "rc.h"

static bool fits(Chunk c, int bytes) {
    return c && (c->size >= bytes && c->free);
    }

static Chunk split(Chunk c, int bytes) {
    if (c->size > bytes) {
        Chunk temp;
        temp->start = c->start + bytes;
        temp->size  = c->size  - bytes - sizeof(chunk);
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
        }
    c->free = 0;
    c->size = bytes;

    c->next = (c->start + c->size);

    memcpy(c->next, temp, sizeof(temp));

    // remove C from freelist

    return Chunk c;
    }

Chunk init(unsigned int bytes) {

    char *memory = (char*) malloc(bytes);

    Chunk temp;
    temp->start = memory;
    temp->size  = bytes;
    temp->next  = NULL;
    temp->free  = 1;
    temp->refcount = 1;
    temp->markbit = 1;

    while (bytes) memory[--bytes] = 0;

    memcpy(memory, temp, sizeof(temp));

    return (Chunk) memory;
    }

Chunk balloc(Memory mem, chunk_size bytes) {
    Chunk c = ((Chunk) ((char*) mem)-sizeof(void*))->data;
// Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    freelist list = ((Chunk) ((char*) mem)-sizeof(void*))->freelist;
    while (!fits(list->current, bytes+sizeof(chunk))) list = list->after; // 

    if (list) {
        return split(list->current, bytes);
        }
    else {
        return NULL;
        }
    }

void whatSort (int flags) {

    if (flags & ADDRESS) {
        return address_free;
    }
    else if (flags & DESCENDING_SIZE) {
        return descending_free;
    }
    else {
        return ascending_free;
    }
}

struct style *iMalloc(unsigned int memsiz, unsigned int flags) {
// Ignoring free list ordering in this simple example
    
    if (flags <= 12) {
        private_manual *mgr = malloc(sizeof(private_manual));
        mgr->data = init(memsiz);
        mgr->functions->alloc = balloc;
        mgr->functions->avail = avail;
        mgr->functions->free = whatSort(flags - 8);

        return &(mgr->functions);
    }
    else if (flags <= 52) {
        private_managed *mgr = malloc(sizeof(private_managed));
        mgr->data = init(memsiz);
        mgr->functions->alloc = balloc;
        
        else if (flags >= 48){
            mgr->functions->rc->retain = increaseReferenceCounter;
            mgr->functions->rc->release = decreaseReferenceCounter;
            mgr->functions->rc->count = returnReferenceCounter;

            mgr->functions->gc = ; // NEVER FORGET
            mgr->functions->gc = ;

        }
        else if (flags >= 32){
            mgr->functions->gc = ;
            mgr->functions->gc = ;

            mgr->functions->rc = NULL;
        }
        else {
            mgr->functions->rc->retain = increaseReferenceCounter;
            mgr->functions->rc->release = decreaseReferenceCounter;
            mgr->functions->rc->count = returnReferenceCounter;

            mgr->functions->gc = NULL;
        }

        return &(mgr->functions);
    }
    else {
        return NULL;
    }   
}