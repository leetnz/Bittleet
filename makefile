FLAGS = -Wall -Icatch

G++ = g++

TESTS = test/TestMain.o \
		test/templateTest.o
SOURCE = 

.PHONY: all
all: test

test: bittleet_tests runTest

.PHONY: runTest
runTest:
	./bittleet_tests

.PHONY: clean
clean:
	rm -f ./**/*.o

%.o: %.cpp
	$(G++) $(FLAGS) -c -o $@ $<

bittleet_tests: $(TESTS) $(SOURCE)
	$(G++) $(FLAGS) $^ -o $@