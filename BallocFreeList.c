
#ifndef __balla
#define __balla
#include "priv_imalloc.h"
#include "manual.h"

void RemoveFromFreelist(Memory mem, Chunk c) {
    private_manual *d = (private_manual*) (((void*)mem) - (3*sizeof(void*)));
    Metafreelist flist = d->flist;
    Freelist list = flist->first;
    if (list->current->start == c->start) {
        d->flist->first = list->after;
        }
    else {
        Freelist prev = list;
        while (list) {
            if(list->current->start == c->start) {
                prev->after = list->after;

                }
            else {
                prev = list;
                list = list->after;

                }
            }
        }
    }

void InsertFreeList(Memory mem, Chunk c) {
    Metafreelist list = (((void*)mem)-(sizeof(void*)));
    int sortType = list->listType;
    if (sortType==1) {
        ascending_free(mem, c);
    }
    else if (sortType==2) {
        descending_free(mem, c);
    }
    else if (sortType==4) {
        adress_free(mem, c);
    }
}


#endif