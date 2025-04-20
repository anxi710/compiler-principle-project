CC = clang++
CXXFLAGS = -Wall -Wextra -Wno-register

DEBUG ?= 1
ifeq ($(DEBUG), 0)
CXXFLAGS += -O2
else
CXXFLAGS += -ggdb -O0
endif

test_token: test_token.cpp  ../src/token.cpp ../src/token_type.cpp ../src/toy_lexer.cpp ../src/lexer.cpp
	@mkdir -p build
	$(CC) $(CXXFLAGS) -I../src $^ -o build/$@

test_keyword_table: test_keyword_table.cpp ../src/token.cpp ../src/token_type.cpp
	@mkdir -p build
	$(CC) $(CXXFLAGS) -I../src $^ -o build/$@

.PHONY: clean

clean:
	rm -rf build