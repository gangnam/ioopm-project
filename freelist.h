#ifndef __manual_h
#define __manual_h
#include "priv_imalloc.h"


/* Kollar hur mycket plats det finns ledigt i det stora minnet, 
eller om vi måste skapa ett nytt stort minnes utrymme. */
unsigned int avail(Memory mem);


/*Letar efter rätt plats att placera chunken i Freelistan, 
denna funktion sorterar enligt ascending, dvs minst först 
och störst sist. */
unsigned int ascending_free(Memory mem, void *ptr);


/* Tar in en chunk och letar efter rätt plats att placera den i 
freelistan enligt descending sortering. Dvs största chunken skall vara först i listan och minst chunken skall vara sist */
unsigned int descending_free(Memory mem, void *ptr);


/* Tar in en chunk och letar efter rätt plats att placera den 
i freelistan, sorteras efter adresstorlek */
unsigned int adress_free(Memory mem, void *ptr);

//static void *__sfreeSimple(void* address);
//static void combine(Memory mem);
//static void *__sfreeDefrag(void* address);


/* Lägger in en chunk i freelistan, chunken är markerad för 
vilken sorering den har. Markeringen avgör vilken sortering 
den får vid insättning i freelistan. 
ascending = 1, descending = 2 och  adress = 4. */
void InsertFreeList(Memory mem, Chunk c);

/*Tar bort en chunk ur freelistan och pekar om*/
void RemoveFromFreelist(Memory mem, Chunk c);


/*Letar igenom freelistan efter en chunk som skall tas bort. Markeras till 1 för att sedan länka ur de chunkar som har en 1 markering. Slår sedan ihop tillgängliga lediga utrymmen efter urlänkning. */
Chunk combine(Memory mem, Chunk original);

#endif
