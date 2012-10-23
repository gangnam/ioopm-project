#ifndef __priv_imalloc_h
#define __priv_imalloc_h
#include <stdbool.h>
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


typedef struct private_manual {
    void  *data;
    manual functions;
    struct freelist *flist;
    } private_manual;

typedef struct private_managed {
    void   *data;
    managed functions;
    struct freelist *flist;
    } private_managed;

typedef struct chunk *Chunk;


struct chunk {
    void* start; // pointer to start of memory
    unsigned size; // how big is this chunk
    Chunk next; // pointer to next chunk
    bool free; // true if the chunk is free, else false
    int refcount; // number of references to the chunk (on the heap or otherwise)
    int markbit; // 1 if there is a reference to this object on the stack
    };

typedef struct freelist {
    Chunk current;
    struct freelist *after;
    } freelist, *Freelist;

#define cSTART(ptr) ptr - sizeof(chunk);

#endif
