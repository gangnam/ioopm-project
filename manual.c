#include <ctype.h>
#include <stdio.h>
#include "imalloc.h"
#include "priv_imalloc.h"

static void combine(Memory mem) {
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Chunk c = d->data;
    while (c->next) {
        if (c->free) {
            if (c->next->free) {
                c->size += c->next->size+sizeof(c->next);
                c->next = c->next->next;
                }
            else {
                c = c->next;
                }
            }
        else {
            c = c->next;
            }
        }
    }

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
    Chunk temp = NULL;
    c->free = 1;
    combine(mem);
    for(; list->current->size < c->size; list = list->after);
    Freelist new;
    new->current = c;
    new->after = list->after;
    list->after = new;

    return &(d->flist->current); //fel return?
    }

///////////////

unsigned int descending_free(Memory mem, void *ptr) {
// Back up one pointer in memory to access the first chunk
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list = d->flist;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    Chunk temp = NULL;
    c->free = 1;
    combine(mem);
    for(; list->current->size > c->size; list = list->after);
    Freelist new;
    new->current = c;
    new->after = list->after;
    list->after = new;
    return &(d->flist->current);
    }

///////////////

unsigned int adress_free(Memory mem, void *ptr) {
// Back up one pointer in memory to access the first chunk
    private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list = d->flist;
    Chunk c = (Chunk) (&ptr-sizeof(struct chunk));
    Freelist temp = NULL;

    c->free = 1;
    combine(mem);

    for(; list->current->start < c->start; list = list->after);
    Freelist new;
    new->current = c;
    new->after = list->after;
    list->after = new;
    
    
    return &(d->flist->current); //fel return?
    }


int main() {
    int b = typeReader("**4\0");
    printf("answer: %d\n",b);
    b = typeReader("4l\0");
    printf("answer: %d\n",b);
    b = typeReader("4i\0");
    printf("answer: %d\n",b);
    b = typeReader("4c\0");
    printf("answer: %d\n",b);
    return 0;
    }

