FLAGS = -Wall -Icatch

G++ = g++

TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(patsubst test/%.cpp,obj/test/%.o,$(TEST_SRC))

APP_SRC = $(wildcard src/*.cpp)
APP_OBJ = $(patsubst src/%.cpp,obj/src/%.o,$(APP_SRC))

.PHONY: all
all: test

test: setup bittleet_tests runTest

setup:
	mkdir -p obj/test
	mkdir -p obj/src

.PHONY: runTest
runTest:
	./bittleet_tests

.PHONY: clean
clean:
	rm -fR ./obj

obj/%.o: %.cpp
	$(G++) $(FLAGS) -c -o $@ $<

bittleet_tests: $(TEST_OBJ) $(SOURCE_OBJ)
	$(G++) $(FLAGS) $^ -o $@