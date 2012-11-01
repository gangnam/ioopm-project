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

/*
steg 1 (setZero) : Itererar över listan över samtliga objekt på heapen och sätter mark-biten till 0.
steg 2 (traverseStack(mf)): Söker igenom stacken efter pekare till objekt på heapen, och med 
utgångspunkt från dessa objekt, traverserar heapen och markera alla objekt 
som påträffas genom att mark-biten sätts till 1.
steg 3 (freeObj): Iterera över listan över samtliga objekt 
på heapen och frigör alla vars mark-bit fortfarande är 0.
*/
unsigned int collectGarbage(Memory mem);

/* 
Kollar om rotpekaren i givna adressrymden pekar in i en chunk och markerar
den, den kolla även all data i chunken och antar att det finns en pekare 
som pekar vidare till en annan del på heapen om det finns så går den in och markerar
den chunken också, detta loopas tills den inte hittar ngn mer pekare.
*/
void mf(void *ptr, void *data);

// Frigör alla objekt vars markbitar fortfarande är satta till 0
void freeObj (Memory mem,Chunk c);

#endif

