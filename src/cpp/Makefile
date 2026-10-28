CXX := g++

CXXFLAGS := -std=c++23 -Wall -Wextra -pedantic\
	-I../../modules/tracy/public

LDLIBS :=

ifeq ($(OS),Windows_NT)
    LDLIBS += -lws2_32 -ldbghelp
endif

TARGET := bin/quadtree_search

SRC := \
    src/main.cpp \
    src/quadtree.cpp \
    src/utils.cpp

TRACY_SRC := ../../modules/tracy/public/TracyClient.cpp

.PHONY: release debug profile clean

release: CXXFLAGS += -O3
release: $(TARGET)

debug: CXXFLAGS += -g -O0
debug: $(TARGET)

profile: CXXFLAGS += -O2 -g -DTRACY_ENABLE
profile: SRC += $(TRACY_SRC)
profile: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET) ../../data/example_data.csv

clean:
	rm -f $(TARGET)