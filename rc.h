#ifndef __rc_h_
#define __rc_h_

/* rc.retain
   Ökar referensräknaren med ett och returnerar hur många referenser den har. */
unsigned int increaseReferenceCounter (void *ptr);

/* rc.release
   Minskar referensräknaren med ett returnerar tillbaka hur många referenser den har till chunken. Om referensräknaren går ner till 0 så tas chunken bort och läggs till free-listan. */
unsigned int decreaseReferenceCounter (Memory mem, void *ptr);

/* rc.count
   returnerar antal referenser till chunken. */
unsigned int returnReferenceCounter (void *ptr);
#endif
