
#include <string.h>
#include "CUnit/Basic.h"

#include "imalloc.h"
//#include "manage.h"
#include "rc.h"
#include "manual.h"
#include "priv_imalloc.h"


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
  CU_ASSERT(c->size == (1 Mb - sizeof(chunk)));
  CU_ASSERT(c->free == 1);
  CU_ASSERT(c->start == (c+sizeof(chunk)));
  
}
void testMANUAL_ADDRESS() {

  Manual mem = (Manual) iMalloc(1 Mb, MANUAL + ADDRESS);
  CU_ASSERT(mem->free == adress_free);
  CU_ASSERT(mem->avail == avail);
  CU_ASSERT(mem->alloc == balloc);
  private_manual *temp = (private_manual*) (((void*) mem)-sizeof(private_manual));
  Chunk c = temp->data;
  CU_ASSERT(c->size == (1 Mb - sizeof(chunk)));
  CU_ASSERT(c->free == 1);
  CU_ASSERT(c->start == (c+sizeof(chunk)));

}

void testGCD_REFCOUNT_DESCENDING()
{
  Managed mem = (Managed) iMalloc(1 Mb, GCD + REFCOUNT + DESCENDING_SIZE);
  CU_ASSERT(mem->rc != NULL);
  CU_ASSERT(mem->rc.release == decreaseReferenceCounter);
  CU_ASSERT(mem->rc.retain == increaseReferenceCounter);
  CU_ASSERT(mem->rc.count == returnReferenceCounter);
  CU_ASSERT(mem->alloc == balloc);
  CU_ASSERT(mem->gc.alloc == typeReader);
  CU_ASSERT(mem->gc.collect == collectGarbage);
  private_managed *temp = (private_managed*) (((void*) mem)-sizeof(private_managed));
  
  Chunk c = temp->data;
  CU_ASSERT(c->size == (1 Mb - sizeof(chunk)));
  CU_ASSERT(c->free == 1);
  CU_ASSERT(c->start == (c+sizeof(chunk)));
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
	(NULL == CU_add_test(pSuiteMANUAL_ASCENDING, "test of manual + ascending", testMANUAL_ASCENDING))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (
        //lägg till test för REFCOUNT här
        (NULL == CU_add_test(pSuiteMANUAL_ADDRESS, "test of manual + adress", testMANUAL_ADDRESS))
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

