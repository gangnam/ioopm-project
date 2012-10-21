#ifndef __manual__
#define __manual__

#include <ctype.h>
#include <stdio.h>
#include "imalloc.h"
#include "priv_imalloc.h"

/* "TypedAllocator"  Returnerar hur många bytes som ska allokeras. T.ex. "***i" innebär 3 void pekare samt 1 int = 3*sizeof(void*) + sizeof(int) */
int typeReader(char* input) {   
    int multiply = 0, result = 0, i = 0, size;
    while (input[i]) {
        if (isdigit(input[i])) {
            multiply *= 10;
            multiply += (input[i] - 48);
            }
        else {
            if (multiply == 0) {
                multiply = 1;
                }

            switch (input[i]) {

                case '*' : {
                    size = sizeof(void*);
                    break;
                    }
                case 'i' : {
                    size = sizeof(int);
                    break;
                    }
                case 'l' : {
                    size = sizeof(long);
                    break;
                    }
                case 'f' : {
                    size = sizeof(float);
                    break;
                    }
                case 'd': {
                    size = sizeof(double);
                    break;
                    }
                default: {
                    size = sizeof(char);
                    break;
                    }
                }

            result += multiply * size;
            multiply = 0;

            }
        i++;
        }
    return result;
    }

/*kollar hur mycket plats det finns ledigt i det stora minnet, eller om vi måste skapa ett nytt stort minnes utrymme. */
unsigned int avail(Memory mem) {
 struct private_manual *d = (struct private_manual*) (&mem - sizeof(void*));
  Chunk c = d->data;
    int avail = 0;
    for (; c; c = c->next) {
        if (c->free) {
            avail += c->size;
            }
        }
    return avail;
    }

/* Free funktionen,  
*
*
*    KOPIERAT IFRÅN SMALLOC!!!!!!!!!!!!!! 
*
*
*/



unsigned int sfree(Memory mem, void *ptr) {
// Back up one pointer in memory to access the first chunk
Chunk c = (Chunk) ((char*) mem)-sizeof(void*);

}

static void *__sfreeSimple(void* address) {
  Chunk c = H;
  while (c && c->start != address) {
    c = c->next;
  }
  if (c) {
    c->free = 1;
    return c->start;
  } else {
    return NULL; 
  }
}

static void combine(Chunk a, Chunk b) {
  a->size += b->size;
  a->next =  b->next;
  free(b);
}

static void *__sfreeDefrag(void* address) {
  Chunk c = H;
  Chunk p = NULL;
  while (c) {
    if (c->start == address) {
      for (int offset=0; offset < c->size; ++offset) *((char*) c->start+offset) = 0;
      c->free = 1;
      Chunk n = c->next;
      while (n && n->free) {
	combine(c, n);
	n = c->next;
      }
    }
    if (c->free) {
      if (p) {
	combine(p,c);
      } else {
	p = c;
      }
    } else {
      p = NULL;
    }
    c = c->next;
  }
  return c ? address : NULL;
}

void *sfree(void* address) {
  // return __sfreeSimple(address); // Enklare variant som inte defragmenterar
  return __sfreeDefrag(address);
}


























int main() {
    int b = typeReader("20i\0");
    printf("answer: %d\n",b);
    return 0;
    }

#endif
