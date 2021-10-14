FLAGS = -Wall -Itest/catch -Isrc -Itest/mock

G++ = g++

TEST_SRC = $(wildcard test/*.cpp test/**/*.cpp)
TEST_OBJ = $(patsubst test/%.cpp,obj/test/%.o,$(TEST_SRC))

FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))

APP_SRC = $(filter-out $(wildcard src/app/**), $(filter-out src/OpenCat.cpp, $(wildcard src/**/*.cpp)))
APP_OBJ = $(patsubst src/%.cpp,obj/src/%.o,$(APP_SRC))

.PHONY: all
all: test

test: setup bittleet_tests runTest

setup:
	mkdir -p $(sort $(dir $(APP_OBJ) $(TEST_OBJ)))


.PHONY: runTest
runTest:
	./bittleet_tests

.PHONY: clean
clean:
	rm -fR ./obj

obj/%.o: %.cpp
	$(G++) $(FLAGS) -c -o $@ $<

bittleet_tests: $(TEST_OBJ) $(APP_OBJ)
	$(G++) $(FLAGS) $^ -o $@
