#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rc.h"
#include "Garbage.h"
#include "freelist.h"
#include "priv_imalloc.h"
#include "rootset.h"

static bool fits(Chunk c, int bytes) {
    return c && (c->size >= (bytes+sizeof(chunk)) && c->free);
}

static Chunk split(Memory mem, Chunk c, int bytes) {
    
    Chunk temp = (Chunk) malloc(sizeof(chunk));
    temp->start = c->start + bytes;
    temp->size  = c->size  - bytes - sizeof(chunk);
    temp->next = c->next;
    temp->free  = 1;
    temp->refcount = 1;
    temp->markbit = 1;
    temp->combined = 0;

    c->free = 0;
    c->size = bytes;

    c->next = (c->start + c->size);

    memcpy(c->next, temp, sizeof(chunk));

    free(temp);

    RemoveFromFreelist(mem, c);
    InsertFreeList(mem, c->next); // tre olika

    return c;
}

Chunk getChunk(Memory mem, chunk_size bytes) {

    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist test = *meta;

    Freelist list = test->first;
    
    for(;list;list=list->after) {
        if(fits(list->current, bytes=sizeof(chunk))) {
            return list->current;
        }
    }
    return NULL;
}

void *balloc(Memory mem, chunk_size bytes) {
    //Chunk c = mem->data;
    // Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    
    Chunk c = getChunk(mem, bytes);

    if (c) {
        return split(mem, c, bytes)->start;
    }
    else {
        return NULL;
    }
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
        man->functions->avail = avail;
        man->functions->free = whatSort(flags - 8);
        

        Chunk temp = (Chunk) malloc(sizeof(chunk));
        temp->start = (memory+sizeof(private_manual)+sizeof(manual)+sizeof(chunk));
        temp->size  = (memsiz-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;

        memcpy((memory+sizeof(private_manual)+sizeof(manual)), temp, sizeof(chunk));

        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+sizeof(private_manual)+sizeof(manual));
        node->after = NULL;

        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->listType = (flags-8);
        head->first = node;

        man->flist = head;

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

        int i;

        if (flags >= 48) {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = NULL;//typeReader;
            mgr->functions->gc.collect = NULL;// NEVER FORGET
            i = flags-48;
        }
        else if (flags >= 32) {
            mgr->functions->gc.alloc = NULL;//typeReader;
            mgr->functions->gc.collect = NULL;// NEVER FORGET

            mgr->functions->rc.retain = NULL;
            mgr->functions->rc.release = NULL;
            mgr->functions->rc.count = NULL;
            i = flags-32;
        }
        else {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = NULL;
            mgr->functions->gc.collect = NULL;
            i = flags-16;
        }

        Chunk temp = (Chunk) malloc(sizeof(chunk));
        temp->start = (memory+sizeof(private_managed)+sizeof(managed)+sizeof(chunk));
        temp->size  = (memsiz-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;

        memcpy((memory+sizeof(private_managed)+sizeof(managed)), temp, sizeof(chunk));

        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+sizeof(private_managed)+sizeof(managed));
        node->after = NULL;

        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->listType = i;
        head->first = node;
        
        mgr->flist = head;

        return (Memory) (mgr->functions);
    }
    else {
        return NULL;
    }
}
/*
int main(void) {
    Manual mfun = (Manual) iMalloc(1 Kb, 9);
    unsigned int i = mfun->avail(mfun);
    printf("%u\n", i);
    private_manual *temp = (private_manual *) (((void*) mfun)-sizeof(private_managed));
    Chunk c = temp->data;
    void *hej = c->start;
    unsigned int i = temp->functions->rc.count(hej);
    printf("%u\n", i); 
    temp->functions->rc.retain(hej);
    i = temp->functions->rc.count(hej);
    printf("%u\n", i);
    return 0;
}*/
