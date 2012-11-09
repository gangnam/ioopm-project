#ifndef __manual_h
#define __manual_h

/* Returnerar hur mycket plats det finns ledigt i Memory mem */
unsigned int avail(Memory mem);

// Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt ascending,
// dvs minst först och störst sist. Den kollar även ifall ptr's chunk
// kan kombineras för att minska fragmentering i vår minnesyta.
unsigned int ascending_free(Memory mem, void *ptr);

// Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt descending,
// dvs störst först och minst sist. Den kollar även ifall ptr's chunk
// kan kombineras för att minska fragmentering i vår minnesyta.
unsigned int descending_free(Memory mem, void *ptr);

// Lägger pekaren ptr's Chunk i Freelistan, sorterad enligt adresserna,
// dvs chunken med lägst adress först och högst sist. Den kollar även ifall ptr's chunk
// kan kombineras för att minska fragmentering i vår minnesyta.
unsigned int adress_free(Memory mem, void *ptr);

// Lägger in en chunk i freelistan, chunken är markerad för vilken sorering
// den har och markeringen avgör vilken sortering den får vid insättning i freelistan.
// Dessa är: ascending = 1, descending = 2 och  adress = 4.
void InsertFreeList(Memory mem, Chunk c);

// Länkar ur en chunk ur freelistan
void RemoveFromFreelist(Memory mem, Chunk c);

void ifree(Memory mem);

#endif
