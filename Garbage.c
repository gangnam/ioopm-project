#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "priv_imalloc.h"
#include "rootset.h"
#include "freelist.h"

/* "TypedAllocator"  T.ex. "***i\0" innebär 3 void pekare samt 1 int = 3*sizeof(void*) + sizeof(int) */
void *typeReader(Memory mem, char *input) {
    int multiply = 0, i = 0, size;
    unsigned int result = 0; 
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
                case 'c': {
                    size = sizeof(char);
                    }
                default: {
                    size = 0;
                    break;
                    }
                }

            result += multiply * size;
            multiply = 0;

            }
        i++;
        }
    if (multiply != 0) {
        return balloc(mem ,(unsigned int)(result + (multiply*sizeof(char))));
        }
    else {
        return balloc(mem, (unsigned int)result);
        }
    }



void setZero (Chunk c) {
    while (c) {
        c->markbit=0;
        c=c->next;
        }
    }

// Kollar om rotpekaren i givna adressrymden pekar in i en chunk och markerar
// den i sådana fall

    void mf(void *ptr, void *data) {
    Chunk c = data;

    while(c) {
        void *chend = ((c->start)+(c->size));
        if ((void*)c >= ptr && ptr <= chend) {
            c->markbit=1;

            void *i = c->start;
            while(i>chend) {
                mf(i,data);
                i++;
                }
            }
        c=c->next;
        }
    }



void freeObj (Memory mem,Chunk c) {
    while (c) {
        if (c->markbit==0){
          InsertFreeList(mem, c);

        }
        c = c->next;
        }
    }


unsigned int collectGarbage(Memory mem) {
    Chunk c = (Chunk) (((void*)mem) - (3*sizeof(void*)));
    /* Metafreelist list = (((void*)mem)  - (sizeof(void*))); */
    Metafreelist *meta = memToMeta(mem);
    Metafreelist list = *meta;
    if(c) {
        setZero(c);
        AddressSpace as = (AddressSpace) malloc(sizeof(addressspace));
        as->start = (char*) c;
        as->end = (char*)(((void*)mem) + list->size);

        traverseStack(as, mf, c);//as skall vara adressspace
        free (as);
        freeObj(mem, c);
        }
        return 0; // konstig
    }

