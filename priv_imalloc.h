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
/* Backar från mem till Metafreelist */
#define memToMeta(mem) *((Metafreelist*) ((void*) mem-sizeof(void*)))
/* Backar från mem till den första chunken */
#define memToChunk(mem) *((Chunk*) ((void*) mem - (sizeof(void*)*3)))
/* Backar från allocade minnets start till dess chunk */
#define ptrToChunk(ptr) (Chunk) (ptr-sizeof(chunk))
/* Storleken av manual's metadata */
#define manMetaSize (sizeof(private_manual)+sizeof(manual))
/* Storleken av managed's metadata */
#define mgrMetaSize (sizeof(private_managed)+sizeof(managed))

typedef struct freelist *Freelist;

typedef struct metafreelist {
  Freelist first; // Första elementet i freelistan
  int listType; // 1 ascending 2 descending 4 adress
  int size; // Storleken av hela heapen
    } metafreelist , *Metafreelist;


typedef struct private_manual {
    void  *data;
    Manual functions;
    Metafreelist flist;
    } private_manual;

typedef struct private_managed {
    void   *data;
    Managed functions;
    Metafreelist flist;
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
    } freelist;

void *balloc(Memory mem, chunk_size bytes);

#endif
