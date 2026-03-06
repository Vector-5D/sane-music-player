# compilers
CC   := clang
CXX  := clang++

# directories
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
OUTPUT  := $(BIN_DIR)/program

# find all source files
SRCS_C   := $(shell find $(SRC_DIR) -name '*.c')
SRCS_CPP := $(shell find $(SRC_DIR) -name '*.cpp')

# generate object file paths
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS_C))
OBJS += $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS_CPP))

# get flags from pkg-config
TAGLIB_CFLAGS := $(shell pkg-config --cflags taglib)
TAGLIB_LDFLAGS := $(shell pkg-config --libs taglib)

RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib)
RAYLIB_LDFLAGS := $(shell pkg-config --libs raylib)

# compilation flags
CFLAGS := -Wall -Wshadow -Iincl -Isrc/raygui/src --std=c23 $(TAGLIB_CFLAGS) $(RAYLIB_CFLAGS)
CXXFLAGS := -Wall -Wshadow -Iincl -Isrc/raygui/src --std=c++17 $(TAGLIB_CFLAGS) $(RAYLIB_CFLAGS)

# linker flags
LDFLAGS := $(TAGLIB_LDFLAGS) $(RAYLIB_LDFLAGS) -lnfd -lgtk-3 -lgobject-2.0 -lglib-2.0

# default target
all: $(BIN_DIR) $(OBJ_DIR) $(OUTPUT)

# link executable
$(OUTPUT): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# compile C sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# compile C++ sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# create directories if missing
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

