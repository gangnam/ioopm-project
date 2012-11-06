#include <string.h>
#include "CUnit/Basic.h"
#include "imalloc.h"
#include "priv_imalloc.h"
#include "rc.h"
#include "freelist.h"
#include "Garbage.h"
#include "rootset.h"
#include <stdlib.h>


int init_suite_manual(void) {
    return 0;
}

int clean_suite_manual(void) {
    return 0;
}

int init_suite_ref(void) {
    return 0;
}

int clean_suite_ref(void) {
    return 0;
}

int init_suite_gc(void) {
    return 0;
}

int clean_suite_gc(void) {
    return 0;
}
// Skapar minne, med ascending sortering. 
// Skapar en chunk och kollar att den pekar på rätta värden
void testMANUAL_ASCENDING(void) {
    Manual mem = (Manual) iMalloc(1 Mb, MANUAL + ASCENDING_SIZE);
    CU_ASSERT(mem->free == ascending_free);
    CU_ASSERT(mem->avail == avail);
    CU_ASSERT(mem->alloc == balloc);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    Chunk c = temp->data;
    CU_ASSERT(c->size == (1 Mb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(c->free == 1);
    CU_ASSERT(c->markbit == 1);
    CU_ASSERT(c->refcount == 1);
    CU_ASSERT(c->next == NULL);
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
    free(temp);
}
// Skapar minne, med adress sortering. 
// Skapar en chunk och kollar att den pekar på rätta värden
void testMANUAL_ADDRESS() {
    Manual mem = (Manual) iMalloc(1 Mb, MANUAL + ADDRESS);
    CU_ASSERT(mem->free == adress_free);
    CU_ASSERT(mem->avail == avail);
    CU_ASSERT(mem->alloc == balloc);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    Chunk c = temp->data;
    CU_ASSERT(c->size == (1 Mb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(c->free == 1);
    CU_ASSERT(c->markbit == 1);
    CU_ASSERT(c->refcount == 1);
    CU_ASSERT(c->next == NULL);
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
    free(temp);
}

// Skapar minne, med descending sortering. 
// Skapar en chunk och kollar att den pekar på rätta värden
void testMANUAL_DESCENDING() {
    Manual mem = (Manual) iMalloc(1 Mb, MANUAL + DESCENDING_SIZE);
    CU_ASSERT(mem->free == descending_free);
    CU_ASSERT(mem->avail == avail);
    CU_ASSERT(mem->alloc == balloc);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    Chunk c = temp->data;
    CU_ASSERT(c->size == (1 Mb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(c->free == 1);
    CU_ASSERT(c->markbit == 1);
    CU_ASSERT(c->refcount == 1);
    CU_ASSERT(c->next == NULL);
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
}

//skapar minne med descening sortering och en chunk, samt kollar att pekarna för refcount pekar rätt
void testGC_REFCOUNT_DESCENDING() {
    Managed mem = (Managed) iMalloc(1 Mb, GCD + REFCOUNT + DESCENDING_SIZE);
    CU_ASSERT(mem->rc.release == decreaseReferenceCounter);
    CU_ASSERT(mem->rc.retain == increaseReferenceCounter);
    CU_ASSERT(mem->rc.count == returnReferenceCounter);
    CU_ASSERT(mem->alloc == balloc);
    CU_ASSERT(mem->gc.alloc == typeReader);
    CU_ASSERT(mem->gc.collect == collectGarbage);
    private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));

    Chunk c = temp->data;
    CU_ASSERT(c->size == (1 Mb - mgrMetaSize - sizeof(chunk)));
    CU_ASSERT(c->free == 1);
    CU_ASSERT(c->markbit == 1);
    CU_ASSERT(c->refcount == 1);
    CU_ASSERT(c->next == NULL);
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
    free(temp);
}

void testCOLLECTGARBAGE() {
    //kan användas som både ett träd och en lista.
    typedef struct tree *Tree;
    typedef struct tree  {
        int current;
        Tree l;
        Tree r;

    } tree;

// test för att testa GC med 2 "list" noder som båda är pekade på(ingen ska free:as)
    Managed mem = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE); // skapa minnet
    //allokera 2 träd noder
    Tree a = (Tree)(mem->alloc((Memory)mem, sizeof(tree)));
    Tree b = (Tree)(mem->alloc((Memory)mem, sizeof(tree)));
    //kolla om chunken a pekar på chunken b
    CU_ASSERT((ptrToChunk((void*)a))->next == (ptrToChunk((void*)b)));

    //sätt så att trädet a pekar på trädet b
    a->current=1;
    a->l=b;
    a->r=NULL;
    b->current=2;
    b->l=NULL;
    b->r=NULL;

    //kör Garbage collect som i detta fall inte ska free:a någonting.
    mem->gc.collect((Memory) mem);

    Metafreelist list = memToMeta((Memory)mem);
    //sätt "rest" chunken till x
    Chunk x = ((ptrToChunk((void*)b)))->next;
    //kolla så att x är den enda chunken i listan
    CU_ASSERT(list->first->current == x);
    CU_ASSERT(list->first->after == NULL);
//-------------------------------------------------------------
//      Test för att testa för samma fall som innan men när den andra
//      noden inte har ngt som pekar på den

    Managed mem2 = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE);
    (Tree)(mem2->alloc((Memory)mem2, sizeof(tree)));
    a = (Tree)(mem2->alloc((Memory)mem2, sizeof(tree)));
    // sätt x till a -> next som i detta fall är "rest" chunken
    x = ((ptrToChunk((void*)a)))->next;
    //kolla så att x är "rest" chunken
    CU_ASSERT(x->size == (1 Kb - sizeof(chunk)*3 - sizeof(tree)*2 - mgrMetaSize));

    a->current=1;
    a->l=NULL;
    a->r=NULL;

    mem2->gc.collect((Memory) mem2);
    list = memToMeta((Memory)mem2);

    //kolla så att det finns 2 element i
    //free listan(rest chunken och en chunk som är sizeof(tree))
    CU_ASSERT(list->first->current == x);
    //CU_ASSERT(list->first->after->current->size == sizeof(tree));
    //CU_ASSERT(list->first->after->after == NULL);
//-------------------------------------------------------------
//      Test för att testa GC för en trädstruktur
//      där vi vill ta bort en nod mitt i trädet

    Managed mem3 = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE);
    //allokera minne för 5 noder.
    a = (Tree)(mem3->alloc((Memory)mem3, sizeof(tree)));
    b = (Tree)(mem3->alloc((Memory)mem3, sizeof(tree)));
    Tree c = (Tree)(mem3->alloc((Memory)mem3, sizeof(tree)));
    Tree d = (Tree)(mem3->alloc((Memory)mem3, sizeof(tree)));
    Tree e = (Tree)(mem3->alloc((Memory)mem3, sizeof(tree)));

    //fixa så att tädet ser ut såhär
    //      a
    //    b    c
    //  d     e
    // vi tar sedan bort pekningen från a till c så vi har två träd
    // sen tar vi bort pekaren till c.
    a->l=b;
    a->r=NULL;
    b->l=d;
    b->r=NULL;
    c->l=e;
    c->r=NULL;
    c=NULL;

    // nu när vi free:ar så skall c tas bort
    mem3->gc.collect((Memory) mem3);
    list = memToMeta((Memory) mem3);
    Chunk q = (ptrToChunk((void*)b))->next;
    //testa att c är den andra chunken i freelistan efter "rest" chunken
    CU_ASSERT(list->first->after->current == q);
    CU_ASSERT((ptrToChunk((void*)b))->next == q);
    CU_ASSERT(list->first->after->after == NULL);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
temp = (private_managed*) (((void*) mem2)-sizeof(private_managed));
 free(temp);
temp = (private_managed*) (((void*) mem3)-sizeof(private_managed));
 free(temp);
}

void testBALLOC() {
    Managed mem = (Managed) iMalloc(1 Kb, REFCOUNT + DESCENDING_SIZE);
    void *a = mem->alloc((Memory)mem,10);
    void *b = mem->alloc((Memory)mem,25);
    void *c = mem->alloc((Memory)mem,12);
    void *d = mem->alloc((Memory)mem,12);
    void *e = mem->alloc((Memory)mem,12);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    Chunk b1 = (Chunk) (b-sizeof(chunk));
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    Chunk d1 = (Chunk) (d-sizeof(chunk));
    Chunk e1 = (Chunk) (e-sizeof(chunk));
    CU_ASSERT(a1->next == b1);
    CU_ASSERT(b1->next == c1);
    CU_ASSERT(c1->next == d1);
    CU_ASSERT(d1->next == e1);

    Manual mem2 = (Manual) iMalloc(1 Kb, MANUAL + ASCENDING_SIZE);

    a = mem2->alloc((Memory)mem2, (1 Kb - manMetaSize - sizeof(chunk) - 60));
    a1 = (Chunk)(a-sizeof(chunk));
    printf("%d\n", a1->size);

    b = mem2->alloc((Memory)mem2, (1 Kb - manMetaSize - sizeof(chunk)));
    b1 = (Chunk)(b-sizeof(chunk));
    //printf("%d\n", b1->size);

    CU_ASSERT(b == NULL);
    // b1 = (Chunk)(b-sizeof(chunk));
    // printf("%d\n", b1->size);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
temp = (private_managed*) (((void*) mem2)-sizeof(private_managed));
 free(temp);
}

//skapar en minnesyta där freelist finns, vi allokerar plats för 5 chunks i minnesytan 
void testFREELIST_ADDRESS() {
    Manual mem = (Manual) iMalloc(1 Kb,MANUAL + ADDRESS);
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;

    void *a = mem->alloc((Memory)mem,10);
    void *b = mem->alloc((Memory)mem,25);
    void *c = mem->alloc((Memory)mem,12);
    void *d = mem->alloc((Memory)mem,45);
    void *e = mem->alloc((Memory)mem,50);

    Chunk e1 = (Chunk) (e-sizeof(chunk));
    Freelist list = flist->first;
    //kollar att freelistan har formats rätt
    CU_ASSERT(list->current == e1->next);
    CU_ASSERT(list->after == NULL);

    mem->free((Memory) mem, c);
    list = flist->first;
    //kollar att freelistan är rätt efter borttagnig av c
    CU_ASSERT(list->current->start == c);

    mem->free((Memory) mem, d);
    list = flist->first;
    //kollar att den pekar rätt efter borttagning av d, 
    //samt kollar att combine fungerat
    CU_ASSERT(list->current->start == c);
    CU_ASSERT(list->current->next->start == e);
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    //kollar att adresserna är sorterade i rätt ordning
    CU_ASSERT(c < d && d < (c1->start + c1->size));

    mem->free((Memory) mem, a);
    mem->free((Memory) mem, b);
    mem->free((Memory) mem, e);

    list = flist->first;
    //kollar att freelistan är rätt samt hanterar 
    //combine och gränsfallet när allt är fritt
    CU_ASSERT(list->current->start == a);
    CU_ASSERT(list->current->next == NULL);
    CU_ASSERT(list->current->size == (1 Kb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(list->after == NULL);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    free(temp);
}

//skapar en minnesyta där freelist finns, vi allokerar plats för 5 chunks i minnesytan
void testFREELIST_ASCENDING() {
    Manual mem = (Manual) iMalloc(1 Kb,MANUAL + ASCENDING_SIZE);
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;

    void *a = mem->alloc((Memory)mem,10);
    void *b = mem->alloc((Memory)mem,25);
    void *c = mem->alloc((Memory)mem,12);
    void *d = mem->alloc((Memory)mem,45);
    void *e = mem->alloc((Memory)mem,50);

    Chunk e1 = (Chunk) (e-sizeof(chunk));
    Freelist list = flist->first;
    //kollar att freelistan har formats rätt
    CU_ASSERT(list->current == e1->next);
    CU_ASSERT(list->after == NULL);

    mem->free((Memory) mem, c);
    list = flist->first;
    //kollar att freelistan är rätt efter borttagnig av c
    CU_ASSERT(list->current->start == c);

    mem->free((Memory) mem, d);
    list = flist->first;
    //kollar att den pekar rätt efter borttagning av d, 
    //samt kollar att combine fungerat
    CU_ASSERT(list->current->start == c);
    CU_ASSERT(list->current->next->start == e);
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    //kollar att adresserna är sorterade i rätt ordning 
    CU_ASSERT(c < d && d < (c1->start + c1->size));

    mem->free((Memory) mem, a);
    mem->free((Memory) mem, b);
    mem->free((Memory) mem, e);

    list = flist->first;
    //kollar att freelistan är rätt samt hanterar 
    //combine och gränsfallet när allt är fritt 
    CU_ASSERT(list->current->start == a);
    CU_ASSERT(list->current->next == NULL);
    CU_ASSERT(list->current->size == (1 Kb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(list->after == NULL);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    free(temp);
}

//skapar en minnesyta där freelist finns, vi allokerar plats för 5 chunks i minnesytan
void testFREELIST_DESCENDING() {
    Manual mem = (Manual) iMalloc(1 Kb, MANUAL + DESCENDING_SIZE);
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;

    void *a = mem->alloc((Memory)mem,10);
    void *b = mem->alloc((Memory)mem,25);
    void *c = mem->alloc((Memory)mem,12);
    void *d = mem->alloc((Memory)mem,45);
    void *e = mem->alloc((Memory)mem,50);

    Chunk e1 = (Chunk) (e-sizeof(chunk));
    Freelist list = flist->first;
    //kollar att freelistan har formats rätt
    CU_ASSERT(list->current == e1->next);
    CU_ASSERT(list->after == NULL);

    mem->free((Memory) mem, c);
    list = flist->first;
    //kollar att freelistan är rätt efter borttagnig av c
    CU_ASSERT(list->after->current->start == c);

    mem->free((Memory) mem, d);
    list = flist->first;
    //kollar att den pekar rätt efter borttagning av d, 
    //samt kollar att combine fungerat
    CU_ASSERT(list->after->current->start == c);
    CU_ASSERT(list->after->current->next->start == e);     
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    //kollar att adresserna är sorterade i rätt ordning
    CU_ASSERT(c < d && d < (c1->start + c1->size));

    mem->free((Memory) mem, a);
    mem->free((Memory) mem, b);
    mem->free((Memory) mem, e);

    list = flist->first;
    //kollar att freelistan är rätt samt hanterar 
    //combine och gränsfallet när allt är fritt
    CU_ASSERT(list->current->start == a);
    CU_ASSERT(list->current->next == NULL);
    CU_ASSERT(list->current->size == (1 Kb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(list->after == NULL);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    free(temp);
}

void testREFCOUNT() {
    Managed mem = (Managed) iMalloc(1 Mb, REFCOUNT + DESCENDING_SIZE);
    Metafreelist *meta = (Metafreelist*) ((void*) mem-sizeof(void*));
    Metafreelist flist = *meta;

    void *a = mem->alloc((Memory)mem,10);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    Freelist list = flist->first;
    CU_ASSERT(mem->rc.count(a)==1);

    mem->rc.retain(a);

    CU_ASSERT(mem->rc.count(a)==2);
    CU_ASSERT(list->current == a1->next);

    mem->rc.release((Memory)mem, a);
    mem->rc.release((Memory)mem, a);

    CU_ASSERT(mem->rc.count(a)==0);

    list = flist->first;

    CU_ASSERT(list->current == a1);
    CU_ASSERT(a1->next == NULL);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
}



void testGCD_ASCENDING() {
    Managed mem = (Managed) iMalloc(1 Kb, GCD + ASCENDING_SIZE);
    int x = (1 Kb- mgrMetaSize - sizeof(chunk));
    void *a = mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    setZero(a1);
    freeObj((Memory) mem, a1);
    CU_ASSERT(avail((Memory)mem) == x);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
}




void testGCD_ADDRESS() {
    Managed mem = (Managed) iMalloc(1 Kb, GCD + ADDRESS);
    int x = (1 Kb- mgrMetaSize - sizeof(chunk));
    void *a = mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    setZero(a1);
    freeObj((Memory) mem, a1);
    CU_ASSERT(avail((Memory)mem) == x);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
}



void testSETZERO() {
    Managed mem = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE);
    
    //skapar pekare till minnesytor
    void *a = mem->alloc((Memory)mem,10);
    void *b = mem->alloc((Memory)mem,25);
    void *c = mem->alloc((Memory)mem,12);
    void *d = mem->alloc((Memory)mem,45);
    void *e = mem->alloc((Memory)mem,50);
    Chunk a1 = (Chunk) (a-sizeof(chunk));//a1:s metadata
    Chunk b1 = (Chunk) (b-sizeof(chunk));
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    Chunk d1 = (Chunk) (d-sizeof(chunk));
    Chunk e1 = (Chunk) (e-sizeof(chunk));
    //sätter alla chunks markbitar till 0
    setZero(a1); 
    //kollar så att alla markbits är satta till 0
    CU_ASSERT(a1->markbit == 0);
    CU_ASSERT(b1->markbit == 0);
    CU_ASSERT(c1->markbit == 0);
    CU_ASSERT(d1->markbit == 0);
    CU_ASSERT(e1->markbit == 0);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
}

void testFREEOBJ() {
    Managed mem = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE);
    int x = (1 Kb- mgrMetaSize - sizeof(chunk));
    void *a = mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    mem->alloc((Memory)mem,10);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    setZero(a1);
    freeObj((Memory) mem, a1);
    CU_ASSERT(avail((Memory)mem) == x);
private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
 free(temp);
}
//skapar en minnesyta och kollar sedan att det 
//ledigt utrymmet överensstämmer efter insättning av chunks
void testAVAIL() {
    int c = (1 Kb- manMetaSize - sizeof(chunk));
    Manual mem = (Manual) iMalloc(1 Kb, MANUAL + ASCENDING_SIZE);
    CU_ASSERT(mem->avail((Memory) mem) == c);
    void *a = mem->alloc((Memory) mem, 10);
    CU_ASSERT(mem->avail((Memory) mem) == c-10-sizeof(chunk));
    mem->free((Memory) mem,a);
    CU_ASSERT(mem->avail((Memory) mem) == c);
    a = mem->alloc((Memory) mem, 10);
    CU_ASSERT(mem->avail((Memory) mem) == c-10-sizeof(chunk));
    void *b = mem->alloc((Memory) mem,30);
    CU_ASSERT(mem->avail((Memory) mem) == c-10-sizeof(chunk)-30-sizeof(chunk));
    mem->free((Memory) mem,a);
    mem->free((Memory) mem,b);
    CU_ASSERT(mem->avail((Memory) mem) == c);
    a = mem->alloc((Memory) mem, 10);
    b = mem->alloc((Memory) mem, 10);
    void *d = mem->alloc((Memory) mem, 10);
    void *e = mem->alloc((Memory) mem, 10);
    void *f = mem->alloc((Memory) mem, 10);
    void *g = mem->alloc((Memory) mem, 10);
    int x = 60 + (sizeof(chunk)*6);
    CU_ASSERT(mem->avail((Memory) mem) == (c - x));
    mem->free((Memory) mem, a);
    mem->free((Memory) mem, b);
    mem->free((Memory) mem, d);
    mem->free((Memory) mem, e);
    mem->free((Memory) mem, f);
    mem->free((Memory) mem, g);
    CU_ASSERT(mem->avail((Memory) mem) == c);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    free(temp);
}

int main() {
  SET_STACK_BOTTOM CURRENT_SP(__g_stack_bottom__);
  CU_pSuite pSuiteMANAGED_REFCOUNT = NULL;
  CU_pSuite pSuiteMANAGED_GC = NULL;
  CU_pSuite pSuiteMANAGED_GC_REFCOUNT = NULL;

  CU_pSuite pSuiteMANUAL_ADDRESS = NULL;
  CU_pSuite pSuiteMANUAL_ASCENDING = NULL;
  CU_pSuite pSuiteMANUAL_DESCENDING = NULL;
 
    
     
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  pSuiteMANAGED_REFCOUNT = CU_add_suite("REFCOUNT", init_suite_manual, clean_suite_manual);
  if (NULL == pSuiteMANAGED_REFCOUNT) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  pSuiteMANAGED_GC = CU_add_suite("GC", init_suite_manual, clean_suite_manual);
  if (NULL == pSuiteMANAGED_GC) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  pSuiteMANAGED_GC_REFCOUNT = CU_add_suite("GC + REFCOUNT", init_suite_manual, clean_suite_manual);
  if (NULL == pSuiteMANAGED_GC_REFCOUNT) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  pSuiteMANUAL_ADDRESS = CU_add_suite("MANUAL + ADDRESS", init_suite_ref, clean_suite_ref);
  if (NULL == pSuiteMANUAL_ADDRESS) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  pSuiteMANUAL_ASCENDING = CU_add_suite("MANUAL + ASCENDING_SIZE", init_suite_gc, clean_suite_gc);
  if (NULL == pSuiteMANUAL_ASCENDING) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  pSuiteMANUAL_DESCENDING = CU_add_suite("MANUAL + DESCENDING", init_suite_gc, clean_suite_gc);
  if (NULL == pSuiteMANUAL_DESCENDING) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (
      //TEST FÖR MANUAL (FREELIST)
      (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test of Manual_ascending", testMANUAL_ASCENDING)) ||
      (NULL == CU_add_test(pSuiteMANUAL_ADDRESS, "test of Manual_address", testMANUAL_ADDRESS))||
      (NULL == CU_add_test(pSuiteMANUAL_DESCENDING, "test of Manual_descening", testMANUAL_DESCENDING)) ||
      (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test of Avail", testAVAIL)) ||
      (NULL == CU_add_test(pSuiteMANUAL_DESCENDING, "test of Freelist_descending", testFREELIST_DESCENDING))||
      (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test of Freelist_ascending", testFREELIST_ASCENDING))||
      (NULL == CU_add_test(pSuiteMANUAL_ADDRESS, "test of Freelist_address", testFREELIST_ADDRESS))
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (
      //TEST FÖR MANAGED GC
      (NULL == CU_add_test(pSuiteMANAGED_GC, "test of setZero", testSETZERO))||
      (NULL == CU_add_test(pSuiteMANAGED_GC, "test of freeObject", testFREEOBJ)) ||
      (NULL == CU_add_test(pSuiteMANAGED_GC, "test of collectGarbage", testCOLLECTGARBAGE)) 
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
      //TEST FÖR MANAGED REFCOUNT
      (NULL == CU_add_test(pSuiteMANAGED_REFCOUNT, "test of Balloc", testBALLOC)) ||
      (NULL == CU_add_test(pSuiteMANAGED_REFCOUNT, "test of Refcount", testREFCOUNT))
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
      //TEST FÖR MANAGED GC + REFCOUNT 
      (NULL == CU_add_test(pSuiteMANAGED_GC_REFCOUNT, "test of GC+REF", testGC_REFCOUNT_DESCENDING))
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}

