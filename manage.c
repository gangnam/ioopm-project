
#ifndef __manage_c
#define __manage_c

#include <stdlib.h>
#include <stdio.h>


/* "TypedAllocator"  T.ex. "***i\0" innebär 3 void pekare samt 1 int = 3*sizeof(void*) + sizeof(int) */
void *typeReader(Memory mem, char *input) {
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
    return balloc(mem ,result + (multiply*sizeof(char)));
  }
  else {
    return balloc(mem, result);
  }
}




// denna funktion kan inte heta GC
// döper om denna då den finns i imalloc.h
void collectGarbage(Memory mem) {
  Chunk c = mem - sizeof(void*);
  if(c){
    setZero(c);
    adressspace as;
    as.start = c;
    as.end = (mem + mem->size);
    traverseStack(as, mf, c);//as skall vara adressspace
    freeObj(c);
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

  while(c){
    int chend= ((c->size) + c->start); 
      if (c >= ptr && ptr <= chend) {
        c->markbit=1;

        void *i = c->start;
        while(i>chend){
          mf(i,data);   
          i++; 
        }
      }
    c=c->next
  }
}


void freeObj (Chunk c) {
  while (c) {
    c->free = !(c->markbit);  
    c = c->next;
  }
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
