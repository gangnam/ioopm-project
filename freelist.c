#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "priv_imalloc.h"

Chunk combine(Memory mem, Chunk original) {

    void **priv = (void**) ((void*) mem-(sizeof(void*)*3));
    Chunk c = (Chunk) *priv;

    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;

    Freelist list = flist->first;
    
    Chunk e = NULL;
    int i = 0;
    Freelist prev = list;
    while (c->next) {
        if (c->free) {
            if (c->next->free) {
                c->size += (c->next->size+sizeof(chunk));
                c->next->combined = 1;
                c->next = c->next->next;
                c->combined = 1;
                i++;
                e = c;
            }
            else {
                c = c->next;
            }
        }
        else {
            c = c->next;
        }
    }

    if (i>0) {
    
        if (list->current->combined == 1) {
            list->current->combined = 0;
            flist->first = list->after;
            list = list->after;
        }
        else {
            while (list) {
                if(list->current->combined == 1) {
                    /*if(flist->first == list) {
                        flist->first = list->after;
                    }*/
                    list->current->combined = 0;
                    prev->after = list->after;
                    list = list->after;
                }
                else {
                    prev = list;
                    list = list->after;
                }
                if(list && list->after) {
                   if (list->current == list->after->current) list->after = NULL;
                }
            }
        }
        return e;
    }
    else {
        return original;
    }
}

/*kollar hur mycket plats det finns ledigt i det stora minnet, eller om vi måste skapa ett nytt stort minnes utrymme. */
unsigned int avail(Memory mem) {
    private_manual *temp = (private_manual *) (((void*) mem)-sizeof(private_manual));
    Chunk c = temp->data;
    int avail = 0;
    for (; c; c = c->next) {
        if (c->free) {
            avail += c->size;
            }
        }
    return avail;
    }

/* Free Funktionen, */


unsigned int ascending_free(Memory mem, void *ptr) {
    // Back up one pointer in memory to access the first chunk
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;
    Freelist list = flist->first;
    Chunk c = (Chunk) (ptr-sizeof(chunk));
    
    c->free = 1;
    c = combine(mem, c);
    if(flist->first == NULL) {
        Freelist new = (Freelist) malloc(sizeof(freelist));
        new->current = c;
        new->after = NULL;
        flist->first = new;
    }
    else {
        Freelist prev = list;
        Freelist new = (Freelist) malloc(sizeof(freelist));
        
        if (list->current->size > c->size){
          new->current = c;
          new->after = list->after;
          flist->first = new;
          return  0;
        }
        while(list) {
            if (list->current->size < c->size) {
                prev = list;
                list = list->after;
            }
            else {
                new->current = c;
                new->after = list->after;
                list->after = new;
                return 0;
            }
        }
        new->current = c;
        new->after = prev->after;
        prev->after = new;
        return 0;
    }
    return 0;
}

///////////////

unsigned int descending_free(Memory mem, void *ptr) {
   // Back up one pointer in memory to access the first chunk
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;
    Freelist list = flist->first;
    Chunk c = (Chunk) (ptr-sizeof(chunk));
    
    c->free = 1;
    c = combine(mem, c);
    if(flist->first == NULL) {
        Freelist new = (Freelist) malloc(sizeof(freelist));
        new->current = c;
        new->after = NULL;
        flist->first = new;
    }
    else {
        Freelist prev = list;
        Freelist new = (Freelist) malloc(sizeof(freelist));
        
        if (list->current->size < c->size){
          new->current = c;
          new->after = list->after;
          flist->first = new;
          return  0;
        }
        while(list) {
            if (list->current->size > c->size) {
                prev = list;
                list = list->after;
            }
            else {
                new->current = c;
                new->after = list->after;
                list->after = new;
                return  0;
            }
        }
        new->current = c;
        new->after = prev->after;
        prev->after = new;
        return 0;
    }
    return 0;
}

    

///////////////

unsigned int adress_free(Memory mem, void *ptr) {
    // Back up one pointer in memory to access the first chunk
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;
    Freelist list = flist->first;
    Chunk c = (Chunk) (ptr-sizeof(chunk));
    
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new = (Freelist) malloc(sizeof(freelist));

    if (list->current->start > c->start){
        new->current = c;
        new->after = list->after;
        flist->first = new;
        return  0;
    }
    while(list) {
        if (list->current->start < c->start) {
            prev = list;
            list = list->after;
        }
        else {
            new->current = c;
            new->after = list->after;
            list->after = new;
            return  0;
        }
    }
    new->current = c;
    new->after = prev->after;
    prev->after = new;
    return 0;
}


void RemoveFromFreelist(Memory mem, Chunk c) {
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;

    Freelist list = flist->first;
    if (list->current->start == c->start) {
        flist->first = list->after;
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
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist list = *meta;

    int sortType = list->listType;
    if (sortType==1) {
        ascending_free(mem, c->start);
    }
    else if (sortType==2) {
        descending_free(mem, c->start);
    }
    else if (sortType==4) {
        adress_free(mem, c->start);
    }
}
