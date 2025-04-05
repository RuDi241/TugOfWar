# Makefile for multi-target C project

# Compiler and linker settings
CC := gcc
LD := gcc

# Build mode (debug or release)
# Usage: make BUILD=release
BUILD ?= debug

# Define the programs
PROGRAMS := referee player graphics

# Program-specific linker flags
REFEREE_LDFLAGS :=
PLAYER_LDFLAGS :=
GRAPHICS_LDFLAGS := -lglfw -lGL -lm -ldl -lfreetype

# Directories
SRC_DIR := src
INCLUDE_DIR := -I./include -I/usr/include/freetype2
BUILD_ROOT := build
BIN_DIR := bin
BUILD_DIR := $(BUILD_ROOT)/$(BUILD)

# Common compiler flags
COMMON_CFLAGS := -Wall -Wextra $(INCLUDE_DIR)

# Debug and release specific flags
ifeq ($(BUILD),debug)
    CFLAGS := $(COMMON_CFLAGS) -g -O0 -MMD
else
    CFLAGS := $(COMMON_CFLAGS) -O2 -DNDEBUG -MMD
endif

# Rule to make all programs
.PHONY: all
all: $(PROGRAMS)

# Function to get source files for a program
get_sources = $(wildcard $(SRC_DIR)/$(1)/*.c)

# Function to transform source files to object files
src_to_obj = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(1))

# Define program-specific LDFLAGS
referee_ldflags = $(REFEREE_LDFLAGS)
player_ldflags = $(PLAYER_LDFLAGS)
graphics_ldflags = $(GRAPHICS_LDFLAGS)

# Generic rule to build each program
define build_program
$(1): $$(call src_to_obj,$$(call get_sources,$(1)))
	@mkdir -p $$(BIN_DIR)
	$$(LD) $$^ -o $$(BIN_DIR)/$$@ $$($(1)_ldflags)
endef

# Apply the build_program rule to each program
$(foreach prog,$(PROGRAMS),$(eval $(call build_program,$(prog))))

# Rule to compile C sources to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -rf $(BUILD_ROOT)
	rm -rf $(BIN_DIR)

# Include all dependency files
-include $(shell find $(BUILD_DIR) -name "*.d" 2>/dev/null)

# Help rule
.PHONY: help
help:
	@echo "Usage: make [target] [BUILD=mode]"
	@echo ""
	@echo "Targets:"
	@echo "  all        Build all programs (default)"
	@echo "  referee    Build the referee program"
	@echo "  player     Build the player program"
	@echo "  graphics   Build the graphics program (links with -lglfw -lGL)"
	@echo "  clean      Remove all build files"
	@echo "  help       Display this help message"
	@echo ""
	@echo "Build Modes:"
	@echo "  debug      Debug build with no optimization (default)"
	@echo "  release    Release build with optimization"
	@echo ""
	@echo "Example:"
	@echo "  make all BUILD=release    # Build all programs in release mode"
