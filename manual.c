#include <ctype.h>
#include <stdio.h>
#include "imalloc.h"
#include "priv_imalloc.h"

Chunk combine(Memory mem, Chunk original) {
    private_manual *d = (private_manual*) (((void*)mem) - (sizeof(void*)*3));
    Metafreelist flist=d->flist;
    Freelist list = flist->first;
    Chunk c = d->data;
    Chunk e;
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
        }//while
    if (i>0) {
    
    if (list->current->combined == 1) {
      list->current->combined = 0;
      flist->first = list->after;
      list = list->after;
    }
    while (list) {
        if(list->current->combined == 1) {
            list->current->combined = 0;
            prev->after = list->after;
            list = list->after;
            }
        else {
            prev = list;
            list = list->after;
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
  private_manual *d = (private_manual*) (((void*)mem) - (sizeof(void*)*3));
    Metafreelist flist = d->flist;
    Freelist list = flist->first;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new;
    
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

///////////////

unsigned int descending_free(Memory mem, void *ptr) {
   // Back up one pointer in memory to access the first chunk
  private_manual *d = (private_manual*) (((void*)mem) - (sizeof(void*)*3));
    Metafreelist flist = d->flist;
    Freelist list = flist->first;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new;
    
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

    

///////////////

unsigned int adress_free(Memory mem, void *ptr) {
    // Back up one pointer in memory to access the first chunk
  private_manual *d = (private_manual*) (((void*)mem) - (sizeof(void*)*3));
    Metafreelist flist = d->flist;
    Freelist list = flist->first;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new;

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