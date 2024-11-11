//
// Created by sriram on 11/10/24.
//

#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "unity.h"
#include "hash.h"

void setUp(void) {}

void tearDown(void) {}

void test_new_map(void) {

    MAP_T *map = new_custom_map(20, &basic);

    TEST_ASSERT_EQUAL_INT(map_capacity(map), 20);
    TEST_ASSERT_EQUAL_INT(map_len(map), 0);

    map_free(map);
}

void test_upsert(void) {

    MAP_T *map = new_custom_map(3, &basic);

    char key1[] = "A"; // ascii 65 -> hash would be 2
    // test insert

    int value1 = 1;
    map_upsert(map, key1, &value1, sizeof(int));
    int *pValue_1 = map_get(map, key1);

    TEST_ASSERT_NOT_NULL(pValue_1);
    TEST_ASSERT_EQUAL_INT(*pValue_1, value1);
    TEST_ASSERT_EQUAL_INT(map_len(map), 1);

    // test update

    value1 = 20;
    map_upsert(map, key1, &value1, sizeof(int));
    pValue_1 = map_get(map, key1);

    TEST_ASSERT_NOT_NULL(pValue_1);
    TEST_ASSERT_EQUAL_INT(*pValue_1, value1);
    TEST_ASSERT_EQUAL_INT(map_len(map), 1);

    // test collision

    char key2[] = "J"; // ascii 74 -> hash would be 2
    value1 = 25;
    map_upsert(map, key2, &value1, sizeof(int));

    int *pValue_2 = map_get(map, key2);
    pValue_1 = map_get(map, key1);

    TEST_ASSERT_NOT_NULL(pValue_1);
    TEST_ASSERT_NOT_NULL(pValue_2);

    TEST_ASSERT_EQUAL_INT(*pValue_1, 20);
    TEST_ASSERT_EQUAL_INT(*pValue_2, 25);
    TEST_ASSERT_EQUAL_INT(map_len(map), 2);

    // test resize

    char key3[] = "K3"; // ascii 126 -> hash would be 0
    int value3 = 50;
    int state = map_upsert(map, key3, &value3, sizeof(int));

    TEST_ASSERT_EQUAL_INT(map_len(map), 3);
    TEST_ASSERT_EQUAL_INT(map_capacity(map), 3);
    TEST_ASSERT_EQUAL_INT(state, 0);

    char key4[] = "O"; // ascii 79 -> hash would be 1
    int value4 = 108;
    map_upsert(map, key4, &value4, sizeof(int));

    TEST_ASSERT_EQUAL_INT(map_len(map), 4);
    TEST_ASSERT_EQUAL_INT(map_capacity(map), 6);

    TEST_ASSERT_EQUAL_INT(*((int*) map_get(map, key1)), 20);
    TEST_ASSERT_EQUAL_INT(*((int*) map_get(map, key2)), 25);
    TEST_ASSERT_EQUAL_INT(*((int*) map_get(map, key3)), 50);
    TEST_ASSERT_EQUAL_INT(*((int*) map_get(map, key4)), 108);

    map_free(map);
}

void test_get(void) {
    MAP_T *map = new_custom_map(3, &basic);
    char key1[] = "A";
    char value[] = "This is a test string";

    // test insert

    int state = map_upsert(map, key1, value, (strlen(value) + 1) * sizeof(char));
    TEST_ASSERT_EQUAL_INT(state, 0);

    char *pValue_1 = map_get(map, key1);
    TEST_ASSERT_EQUAL_STRING(pValue_1, value);


    char key2[] = "B"; // test empty bucket retrieval

    int *pValue_2 = map_get(map, key2);
    TEST_ASSERT_NULL(pValue_2);

    char key3[] = "J"; // test same bucket retrieval of nonexistent element

    pValue_2 = map_get(map, key3);
    TEST_ASSERT_NULL(pValue_2);

    map_free(map);
}

void test_delete(void) {
    MAP_T *map = new_custom_map(10, &basic);
    char key1[] = "A";

    // remove element from empty bucket

    int state = map_delete(map, key1);
    TEST_ASSERT_EQUAL_INT(state, -1);

    int *value1 = malloc(sizeof(int));

    TEST_ASSERT_NOT_NULL(value1);
    *value1 = 1;

    state = map_upsert(map, key1, value1);
    TEST_ASSERT_EQUAL_INT(state, 0);

    // remove first and only element in bucket

    state = map_delete(map, key1);
    TEST_ASSERT_EQUAL_INT(state, 0);
    TEST_ASSERT_NULL(map_get(map, key1));
    TEST_ASSERT_EQUAL_INT(map_len(map), 0);

    value1 = malloc(sizeof(int));
    *value1 = 1;

    int *value2 = malloc(sizeof(int));
    *value2 = 3;

    state = map_upsert(map, key1, value1);

    char key2[] = "K";
    char key3[] = "U";
    state += map_upsert(map, key2, value2);
    TEST_ASSERT_EQUAL_INT(state, 0);

    // remove nonexistent key from len(bucket) > 1
    TEST_ASSERT_EQUAL_INT(map_delete(map, key3), -1);

    // remove first element in a populated bucket
    state = map_delete(map, key1);
    TEST_ASSERT_EQUAL_INT(state, 0);
    TEST_ASSERT_NULL(map_get(map, key1));

    int *result = map_get(map, key2);
    TEST_ASSERT_EQUAL_INT(*result, *value2);
    TEST_ASSERT_EQUAL_INT(map_len(map), 1);

    // remove nonexistent item when len(bucket) == 1
    state = map_delete(map, key1);
    TEST_ASSERT_EQUAL_INT(state, -1);

    map_free(map);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_new_map);
    RUN_TEST(test_upsert);
    RUN_TEST(test_get);
    // RUN_TEST(test_delete);
    return UNITY_END();
}