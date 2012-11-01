#include <string.h>
#include "CUnit/Basic.h"
#include "imalloc.h"
#include "priv_imalloc.h"
#include "rc.h"
#include "freelist.h"
#include "Garbage.h"
#include "rootset.h"



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

void testMANUAL_ASCENDING(void) {
    Manual mem = (Manual) iMalloc(1 Mb, MANUAL + ASCENDING_SIZE);
    CU_ASSERT(mem->free == ascending_free);
    CU_ASSERT(mem->avail == avail);
    CU_ASSERT(mem->alloc == balloc);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    Chunk c = temp->data;
    CU_ASSERT(c->size == (1 Mb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(c->free == 1);
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
    }

void testMANUAL_ADDRESS() {
    Manual mem = (Manual) iMalloc(1 Mb, MANUAL + ADDRESS);
    CU_ASSERT(mem->free == adress_free);
    CU_ASSERT(mem->avail == avail);
    CU_ASSERT(mem->alloc == balloc);
    private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
    Chunk c = temp->data;
    CU_ASSERT(c->size == (1 Mb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(c->free == 1);
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
    }

void testGCD_REFCOUNT_DESCENDING() {
    Managed mem = (Managed) iMalloc(1 Mb, GCD + REFCOUNT + DESCENDING_SIZE);
    //CU_ASSERT(mem->rc != NULL);
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
    CU_ASSERT(c->start == ((void*)c+sizeof(chunk)));
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
    //CU_ASSERT(a1->next == b1);
    }

void testFREELIST() {
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
    CU_ASSERT(list->current == e1->next);
    CU_ASSERT(list->after == NULL);

    mem->free((Memory) mem, c);
    list = flist->first;
    CU_ASSERT(list->after->current->start == c);

    mem->free((Memory) mem, d);
    list = flist->first;
    CU_ASSERT(list->after->current->start == c);
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    CU_ASSERT(c < d && d < (c1->start + c1->size));

    mem->free((Memory) mem, a);
    mem->free((Memory) mem, b);
    mem->free((Memory) mem, e);

    list = flist->first;
    CU_ASSERT(list->current->start == a);
    CU_ASSERT(list->current->next == NULL);
    CU_ASSERT(list->current->size == (1 Kb - manMetaSize - sizeof(chunk)));
    CU_ASSERT(list->after == NULL);
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
    }

void testSETZERO() {
    Managed mem = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE);

    void *a = mem->alloc((Memory)mem,10);
    void *b = mem->alloc((Memory)mem,25);
    void *c = mem->alloc((Memory)mem,12);
    void *d = mem->alloc((Memory)mem,45);
    void *e = mem->alloc((Memory)mem,50);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    Chunk b1 = (Chunk) (b-sizeof(chunk));  
    Chunk c1 = (Chunk) (c-sizeof(chunk));
    Chunk d1 = (Chunk) (d-sizeof(chunk));
    Chunk e1 = (Chunk) (e-sizeof(chunk));
    setZero(a1);
    CU_ASSERT(a1->markbit == 0);
    CU_ASSERT(b1->markbit == 0);
    CU_ASSERT(c1->markbit == 0);
    CU_ASSERT(d1->markbit == 0);
    CU_ASSERT(e1->markbit == 0);
    }

void testFREEOBJ() {
    Managed mem = (Managed) iMalloc(1 Kb, GCD + DESCENDING_SIZE);
    int x = (1 Kb- mgrMetaSize - sizeof(chunk));
    void *a = mem->alloc((Memory)mem,10);
    Chunk a1 = (Chunk) (a-sizeof(chunk));
    setZero(a1);
    freeObj((Memory) mem, a1);
    CU_ASSERT(avail((Memory)mem) == x);

    }

void testAVAIL() 
{
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
}

int main() {
    CU_pSuite pSuiteGCD_REFCOUNT_DESCENDING = NULL;
    CU_pSuite pSuiteMANUAL_ADDRESS = NULL;
    CU_pSuite pSuiteMANUAL_ASCENDING = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    pSuiteGCD_REFCOUNT_DESCENDING = CU_add_suite("GCD + REFCOUNT + DESCENDING_SIZE", init_suite_manual, clean_suite_manual);
    if (NULL == pSuiteGCD_REFCOUNT_DESCENDING) {
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

    if (

        //lägg till test för GC här
        (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test of manual + ascending", testMANUAL_ASCENDING)) ||
        (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test balloc", testBALLOC)) ||
        (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test avail", testAVAIL)) ||
        (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test freelist", testFREELIST))||
        (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test setZero", testSETZERO))||
        (NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test freeObj", testFREEOBJ))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (
        //lägg till test för REFCOUNT här
        (NULL == CU_add_test(pSuiteMANUAL_ADDRESS, "test of manual + adress", testMANUAL_ADDRESS))||
        (NULL == CU_add_test(pSuiteMANUAL_ADDRESS, "test refcount", testREFCOUNT))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (
        (NULL == CU_add_test(pSuiteGCD_REFCOUNT_DESCENDING, "test of gcd + ref + descending", testGCD_REFCOUNT_DESCENDING))
        //lägg till test för Manual här
    ) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
    }

