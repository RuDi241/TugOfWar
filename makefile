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
    REFEREE_EXE := $(BIN_DIR)/referee_debug
    PLAYER_EXE := $(BIN_DIR)/player_debug
else
    OBJ_DIR := $(RELEASE_DIR)
    GCC_FLAGS := $(RELEASE_FLAGS)
    REFEREE_EXE := $(BIN_DIR)/referee_release
    PLAYER_EXE := $(BIN_DIR)/player_release
endif

# Source and object files
REFEREE_SRCS := $(shell find $(SRC_DIR)/referee -name '*.c')  # Referee-specific source files
PLAYER_SRCS := $(shell find $(SRC_DIR)/player -name '*.c')    # Player-specific source files

REFEREE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(REFEREE_SRCS:.c=.o))
PLAYER_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(PLAYER_SRCS:.c=.o))

# Default target
all: $(REFEREE_EXE) $(PLAYER_EXE)

# Linking
$(REFEREE_EXE): $(REFEREE_OBJS) | $(BIN_DIR)
	@echo "------ Linking $(REFEREE_EXE) ------"
	rm -f $(REFEREE_EXE)
	gcc $(GCC_FLAGS) -o $(REFEREE_EXE) $(REFEREE_OBJS)

$(PLAYER_EXE): $(PLAYER_OBJS) | $(BIN_DIR)
	@echo "------ Linking $(PLAYER_EXE) ------"
	rm -f $(PLAYER_EXE)
	gcc $(GCC_FLAGS) -o $(PLAYER_EXE) $(PLAYER_OBJS)

# Compilation for Referee and Player
$(OBJ_DIR)/player/%.o: $(SRC_DIR)/player/%.c | $(OBJ_DIR)/player
	@echo "------ Compiling $(@) ------"
	rm -f $@
	gcc $(GCC_FLAGS) -c -o $@ $<

$(OBJ_DIR)/referee/%.o: $(SRC_DIR)/referee/%.c | $(OBJ_DIR)/referee
	@echo "------ Compiling $(@) ------"
	rm -f $@
	gcc $(GCC_FLAGS) -c -o $@ $<

# Create necessary directories
$(OBJ_DIR)/referee $(OBJ_DIR)/player $(BIN_DIR):
	mkdir -p $@

# Include dependency files
-include $(OBJ_DIR)/referee/*.d $(OBJ_DIR)/player/*.d

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)