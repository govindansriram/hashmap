//
// Created by sriram on 11/10/24.
//

#include "unity.h"
#include "hash.h"

void setUp(void) {}

void tearDown(void) {}

void test_basic_hash(void) {
    char string_one[] = "AAA";

    TEST_ASSERT_EQUAL_UINT64(basic(string_one), 65*3);

    char string_two[] = "z";
    char string_three[] = "==";

    TEST_ASSERT_EQUAL_UINT64(basic(string_two), basic(string_three)); // testing hash collision
}

void test_djb_hash(void) {
    char string_one[] = "ABA";
    char string_two[] = "BAA";

    TEST_ASSERT_NOT_EQUAL(djb(string_one), djb(string_two));

    char string_three[] = "A";
    TEST_ASSERT_EQUAL_UINT64(djb(string_three), 177638);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_basic_hash);
    RUN_TEST(test_djb_hash);
    return UNITY_END();
}