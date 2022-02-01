testsuite: testsuite.c test.h
	gcc testsuite.c -ggdb -O3 -o testsuite

.PHONY: clean test

test: testsuite
	./testsuite

clean:
	rm -f testsuite
