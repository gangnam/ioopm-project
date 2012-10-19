#include <stdio.h>
#include <stdlib.h>
#include "imalloc.h"
#include "priv_imalloc.h"

static bool fits(Chunk c, int bytes) {
    return c && (c->size >= bytes && c->free);
    }

static void *split(Chunk c, int bytes) {
    if (c->size > bytes) {
        c->next = malloc(sizeof(chunk));
        c->next->start = c->start + bytes;
        c->next->size  = c->size  - bytes;
        c->next->next  = NULL;
        c->next->free  = 1;
        }
    c->free = 0;
    c->size = bytes;
    return (void*) c->start;
    }

Chunk init(unsigned int bytes) {
    char *memory = (char*) malloc(bytes);
    Chunk H = (Chunk) malloc(sizeof(chunk));
    H->start = (void*) memory;
    H->size = bytes;
    H->next = NULL;
    H->free = 1;
    while (bytes) memory[--bytes] = 0;
    return H;
    }

void *balloc(Memory mem, chunk_size bytes) {
    Chunk c = mem->data;
// Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    while (!fits(c, bytes)) c = c->next;
    if (c) {
        return split(c, bytes);
        }
    else {
        return NULL;
        }
    }

unsigned int avail(Memory mem) {
	Chunk c = mem->data;
// Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    int avail = 0;
    for (; c; c = c->next)
        if (c->free) avail += c->size;
    return avail;
    }

unsigned int increaseReferenceCounter (void *ptr) {
	ptr->refcount++;
	return ptr->refcount;
}

unsigned int returnReferenceCounter (void *ptr) {
	return ptr->refcount;
}

struct style *iMalloc(unsigned int memsiz, unsigned int flags) {
// Ignoring free list ordering in this simple example
    if (flags & MANUAL) {
        // Allocate space for the struct of functions and metadata
        struct private_manual *mgr = malloc(sizeof(private_manual));

        // Allocate the space that the memory manager will manage
        mgr->data = init(memsiz);
        // Install the functions
        mgr->functions->alloc = balloc;
        mgr->functions->avail = avail;
        mgr->functions->free = sfree;
        return &(manual->functions);
        }
    // Implement all other cases
    return NULL;
    }