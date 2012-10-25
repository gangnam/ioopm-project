#ifndef __manage_h
#define __manage_h
#include "priv_imalloc.h"

typedef char *RawPtr;
typedef struct {
RawPtr start;
RawPtr end;
} addressspace, *AddressSpace;
typedef

// Traverserar hepaen och sätter alla objekts markbitar till 0
void setZero (Chunk c);

// Kollar om rotpekaren i givna adressrymden pekar in i en chunk och markerar
// den i sådana fall
void (*MarkFun)(void *ptr, void *data);

// Traverserar stacken inom en given adressrymd h, med en valfri funktionspekare
// f(se ovan) och en frivillig pekare p som blir f:s andra argument
void traverseStack(AdressSpace h, Markfun f, void *p);

// Frigör alla objekt vars markbitar fortfarande är satta till 0 
void freeObj (chunk c);

// Själva skräpsamlaren, frigör minne som inte används
void GC(Chunk c);

#endif

