#ifndef __rc_h_
#define __rc_h_
#include "imalloc.h"

/*ökar referens pekaren och retunerar referenspekaren till chunken*/
unsigned int increaseReferenceCounter (void *ptr);

/*Tar bort en pekare och när chunken inte har kvar några referenser 
så läggs chunken i freelistan*/
unsigned int decreaseReferenceCounter (Memory mem, void *ptr);

/*retunerar referenspekare till en chunk */
unsigned int returnReferenceCounter (void *ptr);
#endif
