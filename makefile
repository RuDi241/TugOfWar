# Directories
SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := bin

# Build modes
DEBUG_DIR  := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release

# Default build mode
BUILD_MODE ?= debug

# Compiler flags
DEBUG_FLAGS  := -g -O0 -MMD
RELEASE_FLAGS := -O2 -DNDEBUG -MMD

ifeq ($(BUILD_MODE), debug)
    OBJ_DIR := $(DEBUG_DIR)
    GCC_FLAGS := $(DEBUG_FLAGS)
    EXE := $(BIN_DIR)/main_debug
else
    OBJ_DIR := $(RELEASE_DIR)
    GCC_FLAGS := $(RELEASE_FLAGS)
    EXE := $(BIN_DIR)/main_release
endif

# Source and object files
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(SRCS:.c=.o))

# Default target
all: $(EXE)

# Linking
$(EXE): $(OBJS) | $(BIN_DIR)
	@echo "------ Linking $(EXE) ------"
	rm -f $(EXE)
	gcc $(GCC_FLAGS) -o $(EXE) $(OBJS)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "------ Compiling $(@) ------"
	rm -f $@
	gcc $(GCC_FLAGS) -c -o $@ $<

# Create necessary directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Include dependency files
-include $(OBJ_DIR)/*.d

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
