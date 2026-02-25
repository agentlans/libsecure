NAME     := secure
LIB_NAME := lib$(NAME)

SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := build
BIN_DIR  := bin
TEST_DIR := tests

CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -O2 -I$(INC_DIR) -fPIC
LDFLAGS  := -shared

SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_SRC := $(TEST_DIR)/test_main.c
TEST_BIN := $(BIN_DIR)/test_suite

ifeq ($(OS),Windows_NT)
    DYNAMIC_LIB := $(BIN_DIR)/$(LIB_NAME).dll
    LIBS        := -lbcrypt
    RM          := del /q
    MKDIR       := mkdir
else
    DYNAMIC_LIB := $(BIN_DIR)/$(LIB_NAME).so
    LIBS        := 
    RM          := rm -rf
    MKDIR       := mkdir -p
endif

STATIC_LIB  := $(BIN_DIR)/$(LIB_NAME).a

all: directories static dynamic

directories:
	@$(MKDIR) $(OBJ_DIR)
	@$(MKDIR) $(BIN_DIR)

static: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	@ar rcs $@ $^

dynamic: $(DYNAMIC_LIB)

$(DYNAMIC_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# New: Build and run tests
test: static
	$(CC) $(CFLAGS) $(TEST_SRC) $(STATIC_LIB) -o $(TEST_BIN) $(LIBS)
	@echo "--- Running Tests ---"
	./$(TEST_BIN)

clean:
	$(RM) $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories static dynamic clean test
