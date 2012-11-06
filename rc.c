#include <stdio.h>
#include <stdlib.h>
#include "priv_imalloc.h"
#include "freelist.h"
/* rc.retain
   Ökar referensräknaren med ett och returnerar hur många referenser den har till chunken.
 */
unsigned int increaseReferenceCounter (void *ptr) {
    Chunk temp = ptrToChunk(ptr);
    temp->refcount++;
    return temp->refcount;
    }

/* rc.release
   Minskar referensräknaren med ett och returnerar hur många referenser den har till chunken. Om referensräknaren går ner till 0 så tas chunken bort och läggs till free-listan.
*/
unsigned int decreaseReferenceCounter (Memory mem, void *ptr) {
    Chunk temp = ptrToChunk(ptr);
    temp->refcount--;
    if (temp->refcount == 0) {
        temp->free = 1;
        InsertFreeList(mem, temp);
        }
    return temp->refcount;
    }
/* rc.count
   returnerar antal referenser till chunken.
*/
unsigned int returnReferenceCounter (void *ptr) {
    Chunk temp = ptrToChunk(ptr);
    return temp->refcount;
    }
