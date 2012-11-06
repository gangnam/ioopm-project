#ifndef __manual_h
#define __manual_h

/* Returnerar hur mycket plats det finns ledigt i Memory mem */
unsigned int avail(Memory mem);

/* Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt ascending, dvs minst först och störst sist. */
unsigned int ascending_free(Memory mem, void *ptr);

/* Lägger pekaren ptr's Chunk i freelistan, sorterad enligt descending. Dvs största chunken skall vara först i listan och minst chunken skall vara sist */
unsigned int descending_free(Memory mem, void *ptr);

/* Lägger pekaren ptr's Chunk i freelistan, sorterad efter adresstorlek */
unsigned int adress_free(Memory mem, void *ptr);

/* Lägger in en chunk i freelistan, chunken är markerad för
vilken sorering den har. Markeringen avgör vilken sortering
den får vid insättning i freelistan.
ascending = 1, descending = 2 och  adress = 4. */
void InsertFreeList(Memory mem, Chunk c);

/*Tar bort en chunk ur freelistan och pekar om*/
void RemoveFromFreelist(Memory mem, Chunk c);

#endif
