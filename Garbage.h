#ifndef __garbage_h
#define __garbage_h



/* 
Detta är typedallocator funktionen, så den tar in ett gäng flaggor på formen "3*i"
vilket betyder 3*sizeof(void*) + sizeof(int)
den skickar sedan storleken den räknat ut till balloc som allokerar så mycket minne.
sedan returneras Chunk->start för den chunk vi allokerat(alltså chunkens minne).
*/
void *typeReader(Memory mem, char *input);
// Traverserar hepaen och sätter alla objekts markbitar till 0
void setZero (Chunk c);
unsigned int collectGarbage(Memory mem);

// Kollar om rotpekaren i givna adressrymden pekar in i en chunk och markerar
// den i sådana fall

void mf(void *ptr, void *data);

// Frigör alla objekt vars markbitar fortfarande är satta till 0
void freeObj (Memory mem,Chunk c);

#endif

