#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priv_imalloc.h"
#include "freelist.h"
#include "rc.h"
#include "Garbage.h"
#include "rootset.h"
/* Kollar om c finns, om bytes rymms i c och om c är fri */
static bool fits(Chunk c, int bytes) {
    return c && (c->size >= (bytes+sizeof(chunk)) && c->free);
}
/* Skapar en ny chunk som ligger på c->start + bytes plats, och sätter den som fri. Tar även bort c från free-listan. Returnerar c->start. */
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
/* Letar i free-listan efter en Chunk som rymmer bytes och är fri och returnerar den. Returnerar NULL om den inte hittar någon med plats. */
Chunk getChunk(Memory mem, chunk_size bytes) {

    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;

    for(; list; list=list->after) {
        if(fits(list->current, bytes)) {
            return list->current;
        }
    }
    return NULL;
}
/* Allokerar minne inuti mem av storleken bytes. returnerar en void pekare till starten av minne. Returnerar NULL om den ej rymms. */
void *balloc(Memory mem, chunk_size bytes) {

    Chunk c = getChunk(mem, bytes);

    if (c) {
        return split(mem, c, bytes);
    } else {
        return NULL;
    }
}
/* Avgör vilken sorts free-funktion som skall användas beroende på flags. */
Manipulator whatSort (int flags) {

    if (flags & ADDRESS) {
        return adress_free;
    } else if (flags & DESCENDING_SIZE) {
        return descending_free;
    } else {
        return ascending_free;
    }
    return 0;
}
/* Huvudfunktionen. Returnerar en virtuell heap med storleken memsiz som har olika egenskaper beroende på flags. */
struct style *iMalloc(unsigned int memsiz, unsigned int flags) {

    if (flags <= 12) { // Manual

        int totalSize = memsiz;

        char *memory = (char*) malloc(totalSize); // Allokerar hela heapen
        if (memory == NULL) return NULL;          // Kollar om minnet är slut, returnar NULL om så är fallet.
        while (totalSize) memory[--totalSize] = 0; // Nollställer minnet

        private_manual *man = (private_manual *) memory;
        Manual functions = (Manual) (memory+sizeof(private_manual));

        man = (private_manual *) memory;
        /* Pekar ut vilka funktioner som skall finnas */
        man->data = (void *) (memory+manMetaSize); // Pekar data till första chunken
        man->functions = functions;
        man->functions->alloc = balloc;
        man->functions->avail = avail;
        man->functions->free = whatSort(flags - 8);
        /* Skapar första chunken och sätter den som fri */
        Chunk temp = (Chunk) (memory+manMetaSize);
        temp->start = (memory+manMetaSize+sizeof(chunk));
        temp->size  = (memsiz-manMetaSize-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
        /* Skapar free-listan */
        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+manMetaSize);
        node->after = NULL;
        /* Skapar free-listans metahuvud */
        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->first = node;
        head->listType = (flags-8);
        head->size = memsiz-manMetaSize;

        man->flist = head;

        return (Memory) (man->functions);
    } else if (flags <= 52) { // Managed

        int totalSize = memsiz;
        char *memory = (char*) malloc(totalSize); // Allokerar hela heapen
        if (memory == NULL) return NULL;          // Kollar om minnet är slut, returnar NULL om så är fallet.
        while (totalSize) memory[--totalSize] = 0; // Nollställer minnet

        private_managed *mgr = (private_managed *) memory;
        Managed functions = (Managed) (memory+sizeof(private_managed));

        mgr = (private_managed *) memory;
        /* Sätter alloc till balloc */
        mgr->data = (void *) (memory+mgrMetaSize);
        mgr->functions = functions;
        mgr->functions->alloc = balloc;

        int i;
        /* Väljer vilka funktioner som skall ingå beroende på vad flags är */
        if (flags >= 48) {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = typeReader;
            mgr->functions->gc.collect = collectGarbage;
            i = flags-48;
        } else if (flags >= 32) {
            mgr->functions->gc.alloc = typeReader;
            mgr->functions->gc.collect = collectGarbage;

            mgr->functions->rc.retain = NULL;
            mgr->functions->rc.release = NULL;
            mgr->functions->rc.count = NULL;
            i = flags-32;
        } else {
            mgr->functions->rc.retain = increaseReferenceCounter;
            mgr->functions->rc.release = decreaseReferenceCounter;
            mgr->functions->rc.count = returnReferenceCounter;

            mgr->functions->gc.alloc = NULL;
            mgr->functions->gc.collect = NULL;
            i = flags-16;
        }
        /* Skapar första chunken och sätter den som fri */
        Chunk temp = (Chunk) (memory+mgrMetaSize);
        temp->start = (memory+mgrMetaSize+sizeof(chunk));
        temp->size  = (memsiz-mgrMetaSize-sizeof(chunk));
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
        /* Skapar free-listan */
        Freelist node = (Freelist) malloc(sizeof(freelist));
        node->current = (Chunk) (memory+mgrMetaSize);
        node->after = NULL;
        /* Skapar free-listans metahuvud */
        Metafreelist head = (Metafreelist) malloc(sizeof(metafreelist));
        head->first = node;
        head->listType = i;
        head->size = memsiz-mgrMetaSize;

        mgr->flist = head;

        return (Memory) (mgr->functions);
    } else {
        return NULL;
    }
}
