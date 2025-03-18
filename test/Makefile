# 编译器和编译选项
CC     = gcc
CFLAGS = -Wall -Wextra -I ../src/include

# 路径
SRC_DIR  = ../src
TEST_DIR = .
BUILD_DIR  = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))

TEST_BIN  = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%, $(TEST_SRCS))

# 生成编译文件夹
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 编译源代码
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 编译测试代码
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 生成测试可执行文件
$(TEST_BIN): $(OBJS) $(TEST_OBJS)
	$(CC) $(OBJS) $(TEST_OBJS) -o $(TEST_BIN)

.PHONY: test run clean

# 编译所有测试
test: $(TEST_BIN)

# 运行测试
run: $(TEST_BIN)
	$(TEST_BIN)

# 清理构建文件
clean:
	rm -rf $(BUILD_DIR)
