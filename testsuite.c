#include "test.h"
#include <unistd.h>
#include <stdbool.h>

UNIT_TEST(hello) {
	test_assert(true);
}

UNIT_TEST(goodbye) {
	test_assert(true);
}

UNIT_TEST(loopy) {
	sleep(1);
	test_assert(true);
}
