#include <stdio.h>
#include <stdlib.h>
#include "imalloc.h"
#include "rc.h"
#include "priv_imalloc.h"

unsigned int increaseReferenceCounter (Chunk ptr);

unsigned int decreaseReferenceCounter (Memory mem, Chunk ptr);

unsigned int returnReferenceCounter (Chunk ptr);