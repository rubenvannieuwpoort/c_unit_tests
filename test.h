#include <stdio.h>
#include <stdlib.h>

typedef struct test_node test_node;

struct test_node {
	test_node *next;
	void (*func)(void);
};

extern void add_test(test_node *tn);

#define test_assert(b) do { if (b) { printf("SUCCESS\n"); tests_passed++; } else { printf("FAILED\n"); tests_failed++; } } while(0)

#define UNIT_TEST(f) static void f(void);\
  	static void __attribute__((constructor)) __construct_##f(void) { static test_node tn; tn.next = NULL; tn.func = f ; add_test(&tn); } \
	static void __real_##f(void);\
	static void f(void) { printf("%s... ", __func__); fflush(stdout); __real_##f(); }\
	static void __real_##f(void)


int tests_passed = 0, tests_failed = 0;
test_node *start = 0;

void add_test(test_node *tn) {
    test_node **current = &start;
    while (*current) current = &((*current)->next);
    *current = tn;
}


int main() {
	test_node *current = start;
	while (current) {
		current->func();
		current = current->next;
	}
	int total_tests = tests_passed + tests_failed;
	if (total_tests == 0)
		printf("No tests found\n");
	else
		printf("%i tests passed, %i tests failed\n", tests_passed, tests_failed);
	return tests_failed > 0;
}
