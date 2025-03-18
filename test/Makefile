CC     := gcc
CFLAGS := -Wall -Wextra

SRC_DIR    := ../src
TEST_DIR   := .
BUILD_DIR  := build

TEST_BIN  := \
	$(BUILD_DIR)/testUtils \

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# $(BUILD_DIR) 是一个 order-only 依赖，确保目录存在
$(BUILD_DIR)/testUtils: $(TEST_DIR)/testUtils.c $(SRC_DIR)/utils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: test run clean

# 编译所有测试
test: $(TEST_BIN)

# 运行测试
run: $(TEST_BIN)
	@for test in $(TEST_BIN); do \
		echo "Running $$test:"; \
		$$test; \
		echo; \
	done

# 清理构建文件
clean:
	rm -rf $(BUILD_DIR)
