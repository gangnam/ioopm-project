
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imalloc.h"
#include "rc.h"
//#include "manage.h"
#include "manual.h"
#include "priv_imalloc.h"
static bool fits(Chunk c, int bytes) {
    return c && (c->size >= bytes && c->free);
    }

static Chunk split(Chunk c, int bytes) {
    Chunk temp;
    if (c->size > bytes) {

        temp->start = c->start + bytes;
        temp->size  = c->size  - bytes - sizeof(struct chunk);
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
    // add the splitted chunk to freelist

    return c;
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

void *balloc(Memory mem, chunk_size bytes) {
    //Chunk c = mem->data;
// Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list =   d->flist;
    while (!fits(list->current, bytes+sizeof(struct chunk))) list = list->after; //

    if (list) {
        return split(list->current, bytes)->start;
        }

    return NULL;

    }

Manipulator whatSort (int flags) {

    if (flags & ADDRESS) {
        return adress_free;
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
        private_manual *mgr = (private_manual*) malloc(sizeof(private_manual));
        mgr->data = init(memsiz);
        mgr->functions.alloc = balloc;
        mgr->functions.avail = avail;
        mgr->functions.free = whatSort(flags - 8);

        return (Memory) &(mgr->functions);
        }
    else if (flags <= 52) {
        private_managed *mgr = malloc(sizeof(private_managed));
        mgr->data = init(memsiz);
        mgr->functions.alloc = balloc;

        if (flags >= 48) {
            mgr->functions.rc.retain = increaseReferenceCounter;
            mgr->functions.rc.release = decreaseReferenceCounter;
            mgr->functions.rc.count = returnReferenceCounter;

            mgr->functions.gc.alloc = NULL; // NEVER FORGET
            mgr->functions.gc.collect = NULL;// NEVER FORGET

            }
        else if (flags >= 32) {
            mgr->functions.gc.alloc = NULL;// NEVER FORGET
            mgr->functions.gc.collect = NULL;// NEVER FORGET

            mgr->functions.rc.retain = NULL;
            mgr->functions.rc.release = NULL;
            mgr->functions.rc.count = NULL;
            }
        else {
            mgr->functions.rc.retain = increaseReferenceCounter;
            mgr->functions.rc.release = decreaseReferenceCounter;
            mgr->functions.rc.count = returnReferenceCounter;

            mgr->functions.gc.alloc = NULL; // NEVER FORGET
            mgr->functions.gc.collect = NULL;// NEVER FORGET
            }

        return (Memory) &(mgr->functions);
        }
    else {
        return NULL;
        }
    }
