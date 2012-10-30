#ifndef __priv_imalloc_h
#define __priv_imalloc_h
#include <stdbool.h>
#include "imalloc.h"
/*
 * priv_imalloc.h
 *
 * This file contains private specifications for using the imalloc
 * allocator used as a course project for the 2012 IOOP/M course.
 *
 * Note that this specification might evolve as the project is
 * running, f.ex. due to technical as well as educational bugs and
 * insights.
 *
 * You MAY ADD and ALTER the definitions in this file.
 *
 */


/* Actual return type specifications for iMalloc */
typedef struct freelist *Freelist;

typedef struct private_manual {
    void  *data;
    Manual functions;
    Freelist flist;
    } private_manual;

typedef struct private_managed {
    void   *data;
    Managed functions;
    Freelist flist;
    void *size;
    } private_managed;

typedef struct chunk *Chunk;

struct chunk {
    void* start; // pointer to start of memory
    unsigned size; // how big is this chunk
    Chunk next; // pointer to next chunk
    bool free; // true if the chunk is free, else false
    int refcount; // number of references to the chunk (on the heap or otherwise)
    int markbit; // 1 if there is a reference to this object on the stack
    int combined;
} chunk;

typedef struct freelist {
    Chunk current;
    Freelist after;
    int listType; // ASCENDING_SIZE = 1, DESCENDING_SIZE = 2, ADDRESS = 4
    } freelist;

#define cSTART(ptr) ptr - sizeof(chunk);

#endif
