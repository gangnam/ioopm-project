#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "priv_imalloc.h"
#include "rootset.h"
#include "freelist.h"

/* 
"TypedAllocator" om man skickar in T.ex. "***i\0" innebär 3 void pekare 
samt 1 int = 3*sizeof(void*) + sizeof(int) 
*/
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
/*
Itererar över listan över samtliga objekt på heapen och sätter mark-biten till 0.
*/

void setZero (Chunk c) {
    while (c) {
        c->markbit=0;
        c=c->next;
        }
    }

/*
Kollar om rotpekaren i givna adressrymden pekar in i en chunk och markerar
den, den kolla även all data i chunken och antar att det finns en pekare 
som pekar vidare till en annan del på heapen om det finns så går den in och markerar
den chunken också, detta loopas tills den inte hittar ngn mer pekare.
*/

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

/*
Iterera över listan över samtliga objekt 
på heapen och frigör alla vars mark-bit fortfarande är 0.
*/

void freeObj (Memory mem,Chunk c) {
    while (c) {
        if (c->markbit==0){
          InsertFreeList(mem, c);

        }
        c = c->next;
        }
    }

/*
steg 1 (setZero) : Itererar över listan över samtliga objekt på heapen och sätter mark-biten till 0.
steg 2 (traverseStack(mf)): Söker igenom stacken efter pekare till objekt på heapen, och med 
utgångspunkt från dessa objekt, traverserar heapen och markera alla objekt 
som påträffas genom att mark-biten sätts till 1.
steg 3 (freeObj): Iterera över listan över samtliga objekt 
på heapen och frigör alla vars mark-bit fortfarande är 0.
*/
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

