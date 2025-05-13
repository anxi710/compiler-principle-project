CC = clang++
CXXFLAGS = -Wall -Wextra -Wno-register

DEBUG ?= 1
ifeq ($(DEBUG), 0)
CXXFLAGS += -O2
else
CXXFLAGS += -ggdb -O0
endif

test_rtti: test_rtti.cpp
	@mkdir -p build
	$(CC) $(CXXFLAGS) -I ../src $^ -o build/$@

.PHONY: clean

clean:
	rm -rf build
