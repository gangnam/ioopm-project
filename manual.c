#include <ctype.h>
#include <stdio.h>
#include "imalloc.h"
#include "priv_imalloc.h"

Chunk combine(Memory mem, Chunk orginal) {
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Chunk c = d->data;
    Chunk e;
    int i = 0;
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
        return e;
        }
    else {
        return orginal;
        }
    }



/* static void combine(Memory mem) { */
/*     private_manual *d = (private_manual*) (&mem - sizeof(void*)); */
/*     Chunk c = d->data; */
/*     while (c->next) { */
/*         if (c->free) { */
/*             if (c->next->free) { */
/* 	      c->size += c->next->size+sizeof(c->next); //varför c->next?? */
/*                 c->next = c->next->next; */
/*                 } */
/*             else { */
/*                 c = c->next; */
/*                 } */
/*             } */
/*         else { */
/*             c = c->next; */
/*             } */
/*         } */
/*     } */

/*kollar hur mycket plats det finns ledigt i det stora minnet, eller om vi måste skapa ett nytt stort minnes utrymme. */
unsigned int avail(Memory mem) {
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Chunk c = d->data;
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
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list = d->flist;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    Freelist orginal = list;
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new;
    
    if (list->current->combined == 1) {
      list->current->combined = 0;
      d->flist = list->after;
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
    prev = orginal;
    
    if (orginal->current->size > c->size){
      new->current = c;
      new->after = orginal->after;
      d->flist = new;
      return  (d->flist);
    }
    while(orginal) {
        if (orginal->current->size < c->size) {
            prev = orginal;
            orginal = orginal->after;
            }
        else {
            new->current = c;
            new->after = orginal->after;
            orginal->after = new;
            return  (d->flist);
            }
        }
    new->current = c;
    new->after = prev->after;
    prev->after = new;
    return &(d->flist);
    }

///////////////

unsigned int descending_free(Memory mem, void *ptr) {
   // Back up one pointer in memory to access the first chunk
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list = d->flist;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    Freelist orginal = list;
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new;
    
    if (list->current->combined == 1) {
      list->current->combined = 0;
      d->flist = list->after;
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
    prev = orginal;
    
    if (orginal->current->size < c->size){
      new->current = c;
      new->after = orginal->after;
      d->flist = new;
      return  (d->flist);
    }
    while(orginal) {
        if (orginal->current->size > c->size) {
            prev = orginal;
            orginal = orginal->after;
            }
        else {
            new->current = c;
            new->after = orginal->after;
            orginal->after = new;
            return  &(d->flist);
            }
        }
    new->current = c;
    new->after = prev->after;
    prev->after = new;
    return &(d->flist);
    }

    

///////////////

unsigned int adress_free(Memory mem, void *ptr) {
    // Back up one pointer in memory to access the first chunk
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list = d->flist;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    Freelist orginal = list;
    c->free = 1;
    c = combine(mem, c);
    Freelist prev = list;
    Freelist new;

    if (list->current->combined == 1) {
      list->current->combined = 0;
      d->flist = list->after;
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
    prev = orginal;
    
if (orginal->current->start > c->start){
      new->current = c;
      new->after = orginal->after;
      d->flist = new;
      return  (d->flist);
    }
    while(orginal) {
        if (orginal->current->start < c->start) {
            prev = orginal;
            orginal = orginal->after;
            }
        else {
            new->current = c;
            new->after = orginal->after;
            orginal->after = new;
            return  &(d->flist);
            }
        }
    new->current = c;
    new->after = prev->after;
    prev->after = new;
    return &(d->flist);
    }



int main() {
    return 0;
    }

