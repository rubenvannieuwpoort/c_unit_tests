#include "test.h"
#include <unistd.h>
#include <stdbool.h>

test(hello) {
	assert(true);
}

test(goodbye) {
	assert(true);
}

test(loopy) {
	sleep(1);
	assert(true);
}
