
#include <string.h>
#include "CUnit/Basic.h"

#include "imalloc.h"
//#include "manage.h"
#include "rc.h"
#include "manual.h"
#include "priv_imalloc"


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

void fits(void) {
    chunk c = init(16);
    CU_ASSERT(fits(c,16));
    CU_ASSERT(fits(c,100));
    CU_ASSERT(fits(c,1));
    }


void testGC(void) {


  //testfall för GC


}



int main() {
    CU_pSuite pSuiteGC = NULL;
    CU_pSuite pSuiteref = NULL;
    CU_pSuite pSuiteManual = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    pSuiteManual = CU_add_suite("Manual Suite", init_suite_manual, clean_suite_manual);
    if (NULL == pSuiteManual) {
        CU_cleanup_registry();
        return CU_get_error();
        }
    pSuiteref = CU_add_suite("Managed Ref Suite", init_suite_ref, clean_suite_ref);
    if (NULL == pSuiteref) {
        CU_cleanup_registry();
        return CU_get_error();
        }
    pSuiteGC = CU_add_suite("Managed GC Suite", init_suite_gc, clean_suite_gc);
    if (NULL == pSuiteGC) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (

        //lägg till test för GC här
	(NULL == CU_add_test(pSuiteref, "test of garbage collector()", testGC))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (
        //lägg till test för REF här
        (NULL == CU_add_test(pSuiteref, "test of listinsert()", testLIST_LISTINSERT))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (
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

