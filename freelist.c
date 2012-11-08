#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "priv_imalloc.h"

// Ifall vi har två fria chunks bredvid varandra så vill vi slå ihop de två chunksen
// för att förhindra fragmentering i vår minnesyta. Samt att den ger tillbaka den 
// ihopslagna chunken ifall den har slagits ihop, anars får vi tillbaka den orörda chunken.
Chunk combine(Memory mem, Chunk original) {

    Chunk c = memToChunk(mem);
    Chunk temp = c;
    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;

    Chunk e = original;
    int i = 0;
    Freelist prev = list;

    while (c->next != NULL){
      if(c->combined == 1){
	c->combined = 0;}
      else {
	c = c->next;
      }
      }
    if(c->combined == 1){
	c->combined = 0;
    }
    c = temp;
    while (c->next) {
        if (c->free) {
	  if(c != original){
	    i++;
	  }else{
            c->combined = 1;
            i++;
	  }
            if (c->next->free) {
                c->size += (c->next->size+sizeof(chunk));
                c->next->combined = 1;
                c->next = c->next->next;
                c->combined = 1;
                i++;
                e = c;
            } else {
                c = c->next;
            }
        } else {
            c = c->next;
        }
    }

    if (i>0) {
        if(list == NULL) {
            return e;
        } else {
            if (list->current->combined == 1) {
                while(list) {
                    if (list->current->combined == 1) {
                        list->current->combined = 0;
			free(flist->first);
                        flist->first = list->after;
                        list = list->after;
                    } else {
                        break;
                    }

                }
            }
            while (list) {
                if(list->current->combined == 1) {
                    list->current->combined = 0;
		    free(prev->after);
                    prev->after = list->after;
                    list = list->after;
                } else {
                    prev = list;
                    list = list->after;
                }

            }

            return e;
        }
    } else {

      return e;
    }

}

// Returnerar hur mycket plats det finns ledigt i Memory mem 
unsigned int avail(Memory mem) {

    Chunk c = memToChunk(mem);
    int avail = 0;
    for (; c; c = c->next) {
        if (c->free) {
            avail += c->size;
        }
    }
    return avail;
}

// Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt ascending, 
// dvs minst först och störst sist. Den kollar även ifall ptr's chunk 
// kan kombineras för att minska fragmentering i vår minnesyta. 
unsigned int ascending_free(Memory mem, void *ptr) {

    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;
    Chunk c = ptrToChunk(ptr);

    c->free = 1;
    c = combine(mem, c);
    if(flist->first == NULL) {
        Freelist new = (Freelist) malloc(sizeof(freelist));
        new->current = c;
        new->after = NULL;
        flist->first = new;
    } else {
        Freelist prev = list;
        Freelist new = (Freelist) malloc(sizeof(freelist));

        if (list->current->size > c->size) {
            new->current = c;
            new->after = list->after;
            flist->first = new;
            return  0;
        }
        while(list) {
            if (list->current->size < c->size) {
                prev = list;
                list = list->after;
            } else {
                new->current = c;
                new->after = list->after;
                list->after = new;
                return 0;
            }
        }
        new->current = c;
        new->after = NULL;
        prev->after = new;
        return 0;
    }
    return 0;
}

// Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt descending, 
// dvs störst först och minst sist. Den kollar även ifall ptr's chunk 
// kan kombineras för att minska fragmentering i vår minnesyta.
unsigned int descending_free(Memory mem, void *ptr) {

    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;
    Chunk c = ptrToChunk(ptr);
    c->free = 1;
    c = combine(mem, c);
    if(flist->first == NULL) {
        Freelist new = (Freelist) malloc(sizeof(freelist));
        new->current = c;
        new->after = NULL;
        flist->first = new;
    } else {
        Freelist prev = list;
        Freelist new = (Freelist) malloc(sizeof(freelist));
        if (list->current->size < c->size) {
            new->current = c;
            new->after = list->after;//
            flist->first = new;
            return  0;
        }
        while(list) {
            if (list->current->size > c->size) {
                prev = list;
                list = list->after;
            } else {
                new->current = c;
                new->after = list;//
                prev->after = new;
                return  0;
            }
        }
        new->current = c;
        new->after = NULL;
        prev->after = new;
        return 0;
    }
    return 0;
}

// Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt adresserna, 
// dvs chunken med lägst adress först och högst sist. Den kollar även ifall ptr's chunk 
// kan kombineras för att minska fragmentering i vår minnesyta.
unsigned int adress_free(Memory mem, void *ptr) {

    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;
    Chunk c = ptrToChunk(ptr);

    c->free = 1;
    c = combine(mem, c);
    if(flist->first == NULL) {
        Freelist new = (Freelist) malloc(sizeof(freelist));
        new->current = c;
        new->after = NULL;
        flist->first = new;
    } else {
        Freelist prev = list;
        Freelist new = (Freelist) malloc(sizeof(freelist));

        if (list->current->start > c->start) {
            new->current = c;
            new->after = list->after;
            flist->first = new;
            return  0;
        }
        while(list) {
            if (list->current->start < c->start) {
                prev = list;
                list = list->after;
            } else {
                new->current = c;
                new->after = list->after;
                list->after = new;
                return 0;
            }
        }
        new->current = c;
        new->after = NULL;
        prev->after = new;
        return 0;
    }
    return 0;
}

// Länkar ur en chunk ur freelistan
void RemoveFromFreelist(Memory mem, Chunk c) {

    Metafreelist flist = memToMeta(mem);
    Freelist list = flist->first;

    if (list->current->start == c->start) {
      free(flist->first);
        flist->first = list->after;
    } else {
        Freelist prev = list;
        while (list) {
            if(list->current->start == c->start) {
	      free(prev->after);
                prev->after = list->after;
                list = list->after;
            } else {
                prev = list;
                list = list->after;
            }
        }
    }
}

// Lägger in en chunk i freelistan, chunken är markerad för vilken sorering
// den har och markeringen avgör vilken sortering den får vid insättning i freelistan.
// Dessa är: ascending = 1, descending = 2 och  adress = 4. 
void InsertFreeList(Memory mem, Chunk c) {

    Metafreelist flist = memToMeta(mem);

    int sortType = flist->listType;
    if (sortType==1) {
        ascending_free(mem, c->start);
    } else if (sortType==2) {
        descending_free(mem, c->start);
    } else if (sortType==4) {
        adress_free(mem, c->start);
    }
}
