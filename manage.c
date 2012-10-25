
#ifndef __manage_c
#define __manage_c

#include <stdlib.h>
#include <stdio.h>

/* "TypedAllocator"  Returnerar hur många bytes som ska allokeras. T.ex. "***i\0" innebär 3 void pekare samt 1 int = 3*sizeof(void*) + sizeof(int) */
int typeReader(char *input) {
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
    return balloc(result + (multiply*sizeof(char)));
  }
  else {
    return balloc(result, 0);
  }
}




// denna funktion kan inte heta GC
// döper om denna då den finns i imalloc.h
void collectGarbage(Chunk c) {
    setZero(c);
    //STEG2
    freeObj(c);
    }


//STEG 1:
//void iterateHeap(Heap h)
/*int isMarked(Chunk c) {
  if (c->markbit == 1) {
    retun c->markbit;
  }
  else {
    retun c->markbit;
  }
}
*/

void setZero (Chunk c) {
    // ni kan inte stoppa in minnets adress i en chunk.
    Chunk c=c->start;
    while (c->next) {
        c->markbit=0;
        c=c->next;
        }
    break;
    /* följande bör fungera
    while (c) {
      c->markbit=0;
      c=c->next;
    }
    */
    }


/* chunk* endPtr(chunk c) {
  c=c->start;
  while(c->next) {
    c=c->next;
  }
  *endPtr=(c->start)+(c->size)
    return *endPtr;
}
*/

//STEG 2:
void traverseStack(AdressSpace h, Markfun f, void *p) {
    }
c->start <= r) &&  (c->start+c->size)<=r) {
    }
else
    ta bort



    //STEG 3:
    void freeObj (chunk c) {
    Chunk c=c->start;
    if (c->next) {
        while (c->markbit==0) {
            Chunk temp = c;
            free(c);
            temp = c->next;
            }
        }
    else {
        if (c->markbit==0 ) {
            free(c);
            }
        }
    }








//ska printa ut alla rotpekare
void printPtr(void *ptr, void *ignore) {
    printf("%p\n", ptr);
    }

//traverserar/skannar stacken inom en viss adresssrymd h, där f kan vara
// t.ex. printPtr
traverseStack(AddresSpace h, MarkFun f, void *p) {
    }
/*
Steg 1 Iterera över listan över samtliga objekt på heapen och sätt
mark-biten till 0.
Steg 2 Sök igenom stacken efter pekare till objekt på heapen, och med
utgångspunkt från dessa objekt, traversera heapen och markera
alla objekt som påträffas genom att mark-biten sätts till 1.
Steg 3 Iterera över listan över samtliga objekt på heapen och frigör
alla vars mark-bit fortfarande är 0.


*/
