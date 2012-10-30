#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imalloc.h"
#include "rc.h"
//#include "manage.h"
//#include "manual.h"
#include "priv_imalloc.h"

static bool fits(Chunk c, int bytes) {
    return c && (c->size >= bytes && c->free);
}

static Chunk split(Memory mem, Chunk c, int bytes) {
    Chunk temp; // kanske allokera
    if (c->size > bytes) {

        temp->start = c->start + bytes;
        temp->size  = c->size  - bytes - sizeof(chunk);
        temp->next = c->next;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
    }
    c->free = 0;
    c->size = bytes;

    c->next = (c->start + c->size);

    memcpy(c->next, temp, sizeof(temp));

    free(temp);

    //removeFromFreelist(mem, c);
    //addToFreelist(mem, c->next); // tre olika

    return c;
}
/*
char *init(unsigned int bytes, unsigned int headerSize) {

    int totalSize = bytes+headerSize;
    char *memory = (char*) malloc(totalSize);

    while (totalSize) memory[totalSize--] = 0;

    Chunk temp = (Chunk) malloc(sizeof(chunk));
    temp->start = memory+headerSize+sizeof(chunk);
    temp->size  = bytes;
    temp->next  = NULL;
    temp->free  = 1;
    temp->refcount = 1;
    temp->markbit = 1;

    memcpy((memory+headerSize), temp, sizeof(chunk));

    return memory;
    }
*/
void *balloc(Memory mem, chunk_size bytes) {
    //Chunk c = mem->data;
    // Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    void **temp = (void*) (&mem - sizeof(void*));

    Chunk c = (Chunk) temp;

    //Freelist list = d->flist;
    while (!fits(c, bytes+sizeof(chunk))) c = c->next; //

    if (c) {
        return split(mem, c, bytes)->start;
    }

    return NULL;
}

Manipulator whatSort (int flags) {
/*
    if (flags & ADDRESS) {
        return adress_free;
    }
    else if (flags & DESCENDING_SIZE) {
        return descending_free;
    }
    else {
        return ascending_free;
    }
    */
    return 0;
}

struct style *iMalloc(unsigned int memsiz, unsigned int flags) {
// Ignoring free list ordering in this simple example

    // 1. gör minnet
    // 2. allokera private
    // 3. allokera functions
    // 4. lägg in chunk
    // 5. peka

    if (flags <= 12) {

        int totalSize = memsiz+sizeof(private_manual)+sizeof(manual);
        char *memory = (char*) malloc(totalSize);

        while (totalSize) memory[totalSize--] = 0;
        
        private_manual *man = (private_manual *) malloc(sizeof(private_manual));
        Manual functions = (Manual) malloc(sizeof(manual));

        memcpy(memory, man, sizeof(private_manual));
        memcpy((memory+sizeof(private_manual)), functions, sizeof(manual));

        free(man);
        free(functions);

        man = (private_manual *) memory;

        man->data = (void *) (memory+sizeof(private_manual)+sizeof(manual));
        man->functions = (manual *) (memory+sizeof(private_manual));
        man->functions->alloc = balloc;
        man->functions->avail = NULL;//avail;
        man->functions->free = whatSort(flags - 8);

        Chunk temp = (Chunk) malloc(sizeof(chunk));
        temp->start = (memory+sizeof(private_manual)+sizeof(manual)+sizeof(chunk));
        temp->size  = (memsiz-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;

        memcpy((memory+sizeof(private_managed)+sizeof(managed)), temp, sizeof(chunk));

        return (Memory) (man->functions);
    }
    else if (flags <= 52) {

        int totalSize = memsiz+sizeof(private_managed)+sizeof(managed);
        char *memory = (char*) malloc(totalSize);

        while (totalSize) memory[totalSize--] = 0;
        
        private_managed *mgr = (private_managed *) malloc(sizeof(private_managed));
        Managed functions = (Managed) malloc(sizeof(managed));

        memcpy(memory, mgr, sizeof(private_managed));
        memcpy((memory+sizeof(private_managed)), functions, sizeof(managed));

        free(mgr);
        free(functions);

        mgr = (private_managed *) memory;

        mgr->data = (void *) (memory+sizeof(private_managed)+sizeof(managed));
        mgr->functions = (managed *) (memory+sizeof(private_managed));
        mgr->functions->alloc = balloc;

        if (flags >= 48) {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = NULL;//typeReader;
            mgr->functions->gc.collect = NULL;// NEVER FORGET
        }
        else if (flags >= 32) {
            mgr->functions->gc.alloc = NULL;//typeReader;
            mgr->functions->gc.collect = NULL;// NEVER FORGET

            mgr->functions->rc.retain = NULL;
            mgr->functions->rc.release = NULL;
            mgr->functions->rc.count = NULL;
        }
        else {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = NULL;
            mgr->functions->gc.collect = NULL;
        }

        Chunk temp = (Chunk) malloc(sizeof(chunk));
        temp->start = (memory+sizeof(private_managed)+sizeof(managed)+sizeof(chunk));
        temp->size  = (memsiz-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;

        memcpy((memory+sizeof(private_managed)+sizeof(managed)), temp, sizeof(chunk));

        return (Memory) (mgr->functions);
    }
    else {
        return NULL;
    }
}

int main(void) {
    Memory mfun = iMalloc(1 Kb, 48);
    private_managed *temp = (private_managed *) (((void*) mfun)-sizeof(private_managed));
    Chunk c = temp->data;
    void *hej = c->start;
    unsigned int i = temp->functions->rc.count(hej);
    printf("%u\n", i);
    temp->functions->rc.retain(hej);
    i = temp->functions->rc.count(hej);
    printf("%u\n", i);
    return 0;
}
