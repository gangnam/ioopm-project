
#ifndef __manage
#define __manage

#include <stdlib.h>
#include <stdio.h>
#include "manage.h"



void GC(Chunk c) {
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

void setZero (chunk c) {
  Chunk c=c->start;
  while (c->next) {
    c->markbit=0;  
    c=c->next;
  }
  break;
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

Steg 1 Iterera över listan över samtliga objekt på heapen och sätt
mark-biten till 0.
Steg 2 Sök igenom stacken efter pekare till objekt på heapen, och med
utgångspunkt från dessa objekt, traversera heapen och markera
alla objekt som påträffas genom att mark-biten sätts till 1.
Steg 3 Iterera över listan över samtliga objekt på heapen och frigör
alla vars mark-bit fortfarande är 0.


