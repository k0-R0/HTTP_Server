# Compiler & Flags
CC     := gcc
CFLAGS := -Wall -Wextra -pthread

# Directories
SRC_DIR   := src
BUILD_DIR := build

# Files
SRCS   := $(wildcard $(SRC_DIR)/*.c)
OBJS   := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET := $(BUILD_DIR)/server

# Phony Targets
.PHONY: all debug clean

# Default Target
all: $(TARGET)

# Debug Target
debug: CFLAGS += -O0 -g
debug: clean all

# Link Binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile Objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create Build Directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean Build Artifacts
clean:
	rm -rf $(BUILD_DIR)/*
