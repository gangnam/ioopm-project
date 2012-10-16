typedef struct chunk *Chunk;

struct chunk {
	void* start; // pointer to start of memory
	unsigned size; // how big is this chunk
	Chunk next; // pointer to next chunk
	bool free; // true if the chunk is free, else false
};

Chunk init(unsigned int bytes) {
	char *memory = (char*) malloc(bytes);
	Chunk H = (Chunk) malloc(sizeof(chunk));
	H->start = (void*) memory;
	H->size = bytes;
	H->next = NULL;
	H->free = 1;
	while (--bytes) memory[bytes] = 0;
	return H;
}

void *smalloc(Memory mem, chunkSize siz) {
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
// Back up one pointer in memory to access the first chunk Chunk c = (Chunk) ((char*) mem)-sizeof(void*);
	int avail = 0;
	for (; c; c = c->next)
	if (c->free) avail += c->size;
	return avail;
}

struct style *iMalloc(unsigned int memsiz, unsigned int flags) {
// Ignoring free list ordering in this simple example
	if (flags & MANUAL) {
    // Allocate space for the struct of functions and metadata
		struct private_manual *mgr = malloc(sizeof(private_manual));

		// Allocate the space that the memory manager will manage
	    mgr->data = init(memsiz);
	    // Install the functions
		mgr->functions->alloc = smalloc;
		mgr->functions->avail = avail;
		mgr->functions->free = sfree;
		return &(manual->functions);
	}
   // Implement all other cases
	return NULL;
}