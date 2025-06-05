# Flags
CXXFLAGS := -Wall -Wno-register -std=c++23

# Debug flags
DEBUG ?= 1
ifeq ($(DEBUG), 0)
CXXFLAGS += -O2
else
CXXFLAGS += -g -O0
endif

# Compilers
CXX := clang++

# Directories
TARGET_EXEC := toy_compiler
SRC_DIR := src
BUILD_DIR := build

# Source files & target files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(SRCS))

# Header directories & dependencies
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))
DEPS := $(OBJS:.o=.d)
CPPFLAGS = $(INC_FLAGS) -MMD -MP

# Main target
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -lpthread -ldl -o $@

# C++ source
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@) # 确保目录结构正常创建
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

all:
	make -j

bear:
	@make clean
	bear -- make -j

lint:
	@make bear
	clang-tidy -header-filter=.* -p=compile_commands.json \
		-extra-arg=-fdiagnostics-format=msvc \
		$(shell find src -name '*.cpp' -or -name '*.hpp') > .lint.log || true
	bat --style=plain --paging=always .lint.log

format:
	find src -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i

quick-fix:
	nvim -c 'cfile .lint.log | copen'

clean:
	-rm -rf $(BUILD_DIR) compile_commands.json .lint.log *.token *.dot *.symbol

-include $(DEPS)
