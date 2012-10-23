#ifndef __manual_h
#define __manual_h

unsigned int avail(Memory mem);
int typeReader(char* input);
unsigned int ascending_free(Memory mem, void *ptr);
unsigned int descending_free(Memory mem, void *ptr);
unsigned int adress_free(Memory mem, void *ptr);
//static void *__sfreeSimple(void* address);
//static void combine(Memory mem);
//static void *__sfreeDefrag(void* address);
void *sfree(void* address);

#endif