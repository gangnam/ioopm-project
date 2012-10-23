#ifndef __rc_h_
#define __rc_h_

unsigned int increaseReferenceCounter (void *ptr);

unsigned int decreaseReferenceCounter (Memory mem, void *ptr);

unsigned int returnReferenceCounter (void *ptr);
#endif