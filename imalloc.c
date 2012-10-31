#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priv_imalloc.h"
#include "freelist.h"
#include "rc.h"
#include "Garbage.h"
#include "rootset.h"

static bool fits(Chunk c, int bytes) {
    return c && (c->size >= (bytes+sizeof(chunk)) && c->free);
}

void *split(Memory mem, Chunk c, int bytes) {

    Chunk temp = (Chunk) (c->start + bytes);
    temp->start = ((void*)temp + sizeof(chunk));
    temp->size  = (c->size - bytes - sizeof(chunk));
    temp->next = c->next;
    temp->free  = 1;
    temp->refcount = 1;
    temp->markbit = 1;
    temp->combined = 0;

    c->free = 0;
    c->size = bytes;

    c->next = temp;//(Chunk) (c->start + c->size);

    //memcpy(c->next, temp, sizeof(chunk));

    //free(temp);

    InsertFreeList(mem, temp); // tre olika
    RemoveFromFreelist(mem, c);
    
    return c->start;
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
        return split(mem, c, bytes);
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
        
        private_manual *man = (private_manual *) memory;
        Manual functions = (Manual) (memory+sizeof(private_manual));

        //memcpy(memory, man, sizeof(private_manual));
        //memcpy((memory+sizeof(private_manual)), functions, sizeof(manual));

        //free(man);
        //free(functions);

        man = (private_manual *) memory;

        man->data = (void *) (memory+sizeof(private_manual)+sizeof(manual));
        man->functions = functions;//(Manual) (memory+sizeof(private_manual));
        man->functions->alloc = balloc;
        man->functions->avail = avail;
        man->functions->free = whatSort(flags - 8);
        

        Chunk temp = (Chunk) (memory+sizeof(private_manual)+sizeof(manual));
        temp->start = (memory+sizeof(private_manual)+sizeof(manual)+sizeof(chunk));
        temp->size  = (memsiz-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;

        //memcpy((memory+sizeof(private_manual)+sizeof(manual)), temp, sizeof(chunk));

        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+sizeof(private_manual)+sizeof(manual));
        node->after = NULL;

        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->first = node;
        head->listType = (flags-8);
        head->size = memsiz;

        man->flist = head;

        return (Memory) (man->functions);
    }
    else if (flags <= 52) {

        int totalSize = memsiz+sizeof(private_managed)+sizeof(managed);
        char *memory = (char*) malloc(totalSize);

        while (totalSize) memory[totalSize--] = 0;
        
        private_managed *mgr = (private_managed *) memory;
        Managed functions = (Managed) (memory+sizeof(private_managed));

        // memcpy(memory, mgr, sizeof(private_managed));
        // memcpy((memory+sizeof(private_managed)), functions, sizeof(managed));

        // free(mgr);
        // free(functions);

        mgr = (private_managed *) memory;

        mgr->data = (void *) (memory+sizeof(private_managed)+sizeof(managed));
        mgr->functions = (managed *) (memory+sizeof(private_managed));
        mgr->functions->alloc = balloc;

        int i;

        if (flags >= 48) {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = typeReader;
            mgr->functions->gc.collect = collectGarbage;// NEVER FORGET
            i = flags-48;
        }
        else if (flags >= 32) {
            mgr->functions->gc.alloc = typeReader;
            mgr->functions->gc.collect = collectGarbage;// NEVER FORGET

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

        Chunk temp = (Chunk) (memory+sizeof(private_managed)+sizeof(managed));
        temp->start = (memory+sizeof(private_managed)+sizeof(managed)+sizeof(chunk));
        temp->size  = (memsiz-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;

        //memcpy((memory+sizeof(private_managed)+sizeof(managed)), temp, sizeof(chunk));

        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+sizeof(private_managed)+sizeof(managed));
        node->after = NULL;

        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->first = node;
        head->listType = i;
        head->size = memsiz;
        
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
