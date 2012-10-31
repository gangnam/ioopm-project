#ifndef __manage_h
#define __manage_h
#include "priv_imalloc.h"

typedef char *RawPtr;
typedef struct addressspace{
    RawPtr start;
    RawPtr end;
    } addressspace, *AddressSpace;


void *typeReader(Memory mem, char *input);
// Traverserar hepaen och sätter alla objekts markbitar till 0
void setZero (Chunk c);
void collectGarbage(Memory mem);

// Kollar om rotpekaren i givna adressrymden pekar in i en chunk och markerar
// den i sådana fall
void (*MarkFun)(void *ptr, void *data);
void mf(void *ptr, void *data);

// Frigör alla objekt vars markbitar fortfarande är satta till 0
void freeObj (Memory mem,Chunk c);


#endif

