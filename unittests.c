#include <string.h>
#include "CUnit/Basic.h"
#include "imalloc.c"
#include "priv_imalloc.h"

static FILE* temp_file = NULL;

int init_suite_bst(void)
{
  return 0;
}

int clean_suite_bst(void)
{
  return 0;
}

int init_suite_list(void)
{
  return 0;
}

int clean_suite_list(void)
{
  return 0;
}

int init_suite_nw(void)
{
  if (NULL == (temp_file = fopen("temp.txt", "w+")))
    {
      return -1;
    }
  else
    {
      return 0;
    }
}

int clean_suite_nw(void)
{
  if (0 != fclose(temp_file))
    {
      return -1;
    }
  else
    {
      temp_file = NULL;
      return 0;
    }
}

void testBST_INSERT(void)
{
  TreeLink t = insert(NULL, "spam\0", 1);
  CU_ASSERT(strcmp(t->key, "spam\0") == 0);
  CU_ASSERT(t != NULL);
  CU_ASSERT(t->left == NULL);
  CU_ASSERT(t->right == NULL);
  CU_ASSERT(t->rowlist != NULL);
  CU_ASSERT(listlength(t->rowlist) == 1);
  t = insert(t, "spam\0", 2);
  CU_ASSERT(listlength(t->rowlist) == 2);

  TreeLink d = insert(NULL, "ni\0", 1);
  d = insert(d, "spam\0", 2);
  d = insert(d, "eki\0", 3);
  CU_ASSERT(strcmp(d->key, "ni\0") == 0);
  CU_ASSERT(strcmp(d->right->key, "spam\0") == 0);
  CU_ASSERT(strcmp(d->left->key, "eki\0") == 0);
}


int main()
{
  CU_pSuite pSuiteBst = NULL;
  CU_pSuite pSuiteList = NULL;
  CU_pSuite pSuiteNW = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  pSuiteNW = CU_add_suite("nextWord Suite", init_suite_nw, clean_suite_nw);
  if (NULL == pSuiteNW)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
  pSuiteList = CU_add_suite("Linked List Suite", init_suite_list, clean_suite_list);
  

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}

