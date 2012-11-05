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

    c->next = temp;

    InsertFreeList(mem, temp);
    RemoveFromFreelist(mem, c);
    
    return c->start;
}

Chunk getChunk(Memory mem, chunk_size bytes) {

    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;
    
    for(;list;list=list->after) {
        if(fits(list->current, bytes=sizeof(chunk))) {
            return list->current;
        }
    }
    return NULL;
}

void *balloc(Memory mem, chunk_size bytes) {

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

    if (flags <= 12) {

        int totalSize = memsiz;

        char *memory = (char*) malloc(totalSize);

        while (totalSize) memory[--totalSize] = 0;
        
        private_manual *man = (private_manual *) memory;
        Manual functions = (Manual) (memory+sizeof(private_manual));

        man = (private_manual *) memory;

        man->data = (void *) (memory+manMetaSize);
        man->functions = functions;
        man->functions->alloc = balloc;
        man->functions->avail = avail;
        man->functions->free = whatSort(flags - 8);

        Chunk temp = (Chunk) (memory+manMetaSize);
        temp->start = (memory+manMetaSize+sizeof(chunk));
        temp->size  = (memsiz-manMetaSize-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
        temp->combined = 0;

        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+manMetaSize);
        node->after = NULL;

        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->first = node;
        head->listType = (flags-8);
        head->size = memsiz;

        man->flist = head;

        return (Memory) (man->functions);
    }
    else if (flags <= 52) {

        int totalSize = memsiz;
        char *memory = (char*) malloc(totalSize);

        while (totalSize) memory[--totalSize] = 0;

        private_managed *mgr = (private_managed *) memory;
        Managed functions = (Managed) (memory+sizeof(private_managed));

        mgr = (private_managed *) memory;

        mgr->data = (void *) (memory+mgrMetaSize);
        mgr->functions = functions;
        mgr->functions->alloc = balloc;

        int i;

        if (flags >= 48) {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = typeReader;
            mgr->functions->gc.collect = collectGarbage;
            i = flags-48;
        }
        else if (flags >= 32) {
            mgr->functions->gc.alloc = typeReader;
            mgr->functions->gc.collect = collectGarbage;

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

        Chunk temp = (Chunk) (memory+mgrMetaSize);
        temp->start = (memory+mgrMetaSize+sizeof(chunk));
        temp->size  = (memsiz-mgrMetaSize-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
        temp->combined = 0;

        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+mgrMetaSize);
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