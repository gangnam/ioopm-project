#include <stdio.h>
#include <stdlib.h>
#include "imalloc.h"
#include "priv_imalloc.h"

static bool fits(Chunk c, int bytes) {
    return c && (c->size >= bytes && c->free);
    }

static Chunk split(Chunk c, int bytes) {
    if (c->size > bytes) {
        Chunk temp;
        temp->start = c->start + bytes;
        temp->size  = c->size  - bytes - sizeof(chunk);
        temp->next  = NULL;
        temp->free  = 1;
        temp->refcount = 1;
        temp->markbit = 1;
        }
    c->free = 0;
    c->size = bytes;

    c->next = (c->start + c->size);

    memcpy(c->next, temp, sizeof(temp));

    return Chunk c;
    }

Chunk init(unsigned int bytes) {

    char *memory = (char*) malloc(bytes);

    Chunk temp;
    temp->start = memory;
    temp->size  = bytes;
    temp->next  = NULL;
    temp->free  = 1;
    temp->refcount = 1;
    temp->markbit = 1;

    while (bytes) memory[--bytes] = 0;

    memcpy(memory, temp, sizeof(temp));

    return (Chunk) memory;
    }

Chunk balloc(Memory mem, chunk_size bytes) {
    Chunk c = mem->data;
// Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
    while (!fits(c, bytes+sizeof(chunk))) c = c->next;

    if (c) {
        return split(c, bytes);
        }
    else {
        return NULL;
        }
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