# C unit tests

Minimalistic unit tests in C. Uses the `__attribute__((constructor))` which, as far as I know, is supported by `GCC` and `clang`. So this probably only works for those compilers.

![](https://raw.githubusercontent.com/rubenvannieuwpoort/files/master/demonstration.gif)

## Example

To see the unit tests run:

```
$ git clone git@github.com:rubenvannieuwpoort/c_unit_tests.git
$ cd c_unit_tests
$ make test
```


## Usage

Make a separate C source file for your tests that includes `test.h`. This file should not contain a `main` -- it is provided by the `test.h` header file. A unit test now looks like this:
```
UNIT_TEST(test_name) {
	// do something useful here
	test_assert(...);
}
```

This file can be compiled and run as usual. When running, it displays something like
```
test_name... SUCCESS
1 tests passed, 0 tests failed
```

And the program will exit with code 0 if all tests passed and 1 if there are failing tests.

I like to make a target `test` in the `Makefile` that runs the test binary, so that you can do `make test` to run the tests. See this repo for example:
```
$ make test
gcc testsuite.c -ggdb -O3 -o testsuite
./testsuite
hello... SUCCESS
goodbye... SUCCESS
loopy... SUCCESS
3 tests passed, 0 tests failed
```


## How does it work?

Short story, it uses some macro magic.

Long story:
`UNIT_TEST` is a macro. If you write `UNIT_TEST(my_test)`, this line expands to
```
static void my_test(void);

static void __attribute__((constructor)) __construct_my_test(void) {
	add_test(my_test);
}

static void __real_my_test(void);

static void my_test(void) {
	printf("%s... ", __func__);
	fflush(stdout);
	__real_my_test();
}

static void __real_my_test(void)
```

Note that there is intentionally no semicolon on the last line: `UNIT_TEST(my_test)` should be followed by a block that makes up the body of the test.
This code:
  1. Declares `my_test` but doesn't define it (a so-called [forward declaration](https://en.wikipedia.org/wiki/Forward_declaration)). This is necessary so that in the next step, we can use `my_test` without having defined it.
  2. Defines a `__construct_my_test` function. This has the `__attribute__((constructor))` attribute, which means the function is executed *even before `main` is executed* (this is GCC-specific, see [this page](https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Function-Attributes.html) for more information). This code then runs `add_test(my_test)`, which adds a function pointer to `my_test` to a global linked list (defined in `test.h`).
  3. We have another forward declaration, now for `__real_my_test`, the function that will contain the actual unit test code.
  4. It then defines a function `my_test` which prints `my_test... ` and then runs `__real_my_test`.
  5. `__real_my_test` now contains the code block that was placed after `UNIT_TEST(my_test)` as its body.

The `main` method provided by `test.h` traverses the linked list with function pointers that was initialized by the `__construct_my_test` method (and other such methods if you have more testcases), and runs the functions in the linked list.

Then, the `__real_my_test` function is the actual tests which calls `test_assert(condition)`, which, again, is a macro. It expands to:
```
do {
	if (condition) {
		printf("SUCCESS\n");
		tests_passed++;
	}
	else {
		printf("FAILED\n");
		tests_failed++;
	}
} while(0)
```

This `do ... while` is there to make sure the behavior is right when the macro is used in any context (see [this page](http://www.bruceblinn.com/linuxinfo/DoWhile.html) for a more in-depth explanation). The other code is straightforward. It prints `SUCCESS` or `FAILED` (which comes after the `mytest... ` which was printed in `my_test`, assuming that no other stuff is printed before `test_assert` is called) and keeps track of the number of passed and failed tests.

Before terminating, `main` prints the number of tests passed and failed (or `No tests are found` if no tests have run at all).
