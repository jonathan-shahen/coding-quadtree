CXX := g++

MODE ?= release

COMMON_FLAGS := \
	-std=c++23 \
	-MMD -MP \
	-Wall \
	-Wextra \
	-pedantic \
	-I../../modules/tracy/public

BUILD_DIR := build/$(MODE)
TARGET := $(BUILD_DIR)/quadtree_search

SRC := $(wildcard src/*.cpp)

OBJ := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

LDLIBS :=

# --------------------------------------------------
# Build mode configuration
# --------------------------------------------------

CXXFLAGS := $(COMMON_FLAGS)

ifeq ($(MODE),release)
	CXXFLAGS += -O3
endif

ifeq ($(MODE),debug)
	CXXFLAGS += -O0 -g -DDEBUG
endif

ifeq ($(MODE),profile)
	CXXFLAGS += -O2 -g -DTRACY_ENABLE

	TRACY_OBJ := $(BUILD_DIR)/TracyClient.o

	OBJ += $(TRACY_OBJ)

	ifeq ($(OS),Windows_NT)
		LDLIBS += -lws2_32 -ldbghelp -lsecur32
	endif
endif

# --------------------------------------------------
# Default target
# --------------------------------------------------

all: $(TARGET)

# --------------------------------------------------
# Compile project source files
# --------------------------------------------------

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --------------------------------------------------
# Compile Tracy separately
# --------------------------------------------------

$(BUILD_DIR)/TracyClient.o: ../../modules/tracy/public/TracyClient.cpp | $(BUILD_DIR)
	$(CXX) -O2 -g -DTRACY_ENABLE -I../../modules/tracy/public -w -c $< -o $@

# --------------------------------------------------
# Link executable
# --------------------------------------------------

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDLIBS)

# --------------------------------------------------
# Create build directory
# --------------------------------------------------

$(BUILD_DIR):
	mkdir -p $@

# --------------------------------------------------
# Convenience targets
# --------------------------------------------------

release:
	$(MAKE) MODE=release all

debug:
	$(MAKE) MODE=debug all

profile:
	$(MAKE) MODE=profile all

ARGS ?=
run:
	./$(TARGET) ../../data/example_data.csv $(ARGS)

run-release:
	$(MAKE) MODE=release run

run-debug:
	$(MAKE) MODE=debug run

# Add the `-q` flag to automatically quit
run-profile:
	$(MAKE) MODE=profile ARGS="-q" run

clean:
	rm -rf build

.PHONY: all release debug profile run run-release run-debug run-profile clean

-include $(DEP)