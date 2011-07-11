/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * linked_list_test.c
 *
 *  Created on: Jul 16, 2010
 *      Author: alexanderb
 */
#include <stdio.h>
#include <apr_pools.h>

#include "Basic.h"
#include "linkedlist.h"

apr_pool_t *memory_pool;
LINKED_LIST list;

int setup(void) {
    apr_initialize();
    apr_pool_create(&memory_pool, NULL);

    linkedList_create(memory_pool, &list);
    if (list) {
        return 0;
    } else {
        // failure during setup
        return 1;
    }
}

int teardown(void) {
    apr_pool_destroy(memory_pool);
    apr_terminate();
    return 0;
}

void test_linkedList_create(void) {
    CU_ASSERT_PTR_NOT_NULL_FATAL(list);
    CU_ASSERT_EQUAL(linkedList_size(list), 0);
}

void test_linkedList_add(void) {
    CU_ASSERT_EQUAL(linkedList_size(list), 0);
    linkedList_addElement(list, "element");
	CU_ASSERT_EQUAL(linkedList_size(list), 1);
}

void test_linkedList_remove(void) {
    CU_ASSERT_EQUAL(linkedList_size(list), 0);
    linkedList_addElement(list, "element");
    CU_ASSERT_EQUAL(linkedList_size(list), 1);

    linkedList_removeElement(list, "element");
    CU_ASSERT_EQUAL(linkedList_size(list), 0);
}

int main (int argc, char** argv) {
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
	  return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_1", setup, teardown);
	if (NULL == pSuite) {
	  CU_cleanup_registry();
	  return CU_get_error();
	}

	/* add the tests to the suite */
	if (NULL == CU_add_test(pSuite, "List Creation Test", test_linkedList_create) ||
		NULL == CU_add_test(pSuite, "List Add Test", test_linkedList_add) ||
		NULL == CU_add_test(pSuite, "List Remove Test", test_linkedList_remove))
	{
	  CU_cleanup_registry();
	  return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}