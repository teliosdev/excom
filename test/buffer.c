
#include <stdlib.h>
#include <stdio.h>
#define __FILE "buffer.test"

#define TEST_PREFIX
#include "./utest.h"
#include "./buffer.test"
#undef TEST_PREFIX

int main() {
#define TEST_BEFORE
#include "./utest.h"
TEST_BEFORE_HOOK1;
#include "./buffer.test"
TEST_BEFORE_HOOK2;
#undef TEST_BEFORE

#define TEST_DURING
#include "./utest.h"
#include "./buffer.test"
#undef TEST_DURING

#define TEST_AFTER
#include "./utest.h"
TEST_AFTER_HOOK1;
#include "./buffer.test"
TEST_AFTER_HOOK2;
#undef TEST_AFTER

  return TEST_RETURN;
}

