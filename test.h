#include <stdio.h>
#include <stdlib.h>

extern void add_test(void (*f)(void));

#define test_assert(b) do { if (b) { printf("SUCCESS\n"); tests_passed++; } else { printf("FAILED\n"); tests_failed++; } } while(0)

#define UNIT_TEST(f) static void f(void);\
	static void __attribute__((constructor)) __construct_##f(void) { add_test(f); }\
	static void __real_##f(void);\
	static void f(void) { printf("%s... ", __func__); fflush(stdout); __real_##f(); }\
	static void __real_##f(void)

typedef struct test_node test_node;

struct test_node {
	test_node *next;
	void (*func)(void);
};

int tests_passed = 0, tests_failed = 0;
test_node *start = 0;

void add_test(void (*f)(void)) {
	test_node **current = &start;
	while (*current) current = &((*current)->next);
	*current = malloc(sizeof(test_node));
	(*current)->next = 0;
	(*current)->func = f;
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
