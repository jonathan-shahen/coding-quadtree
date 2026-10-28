CXX := g++

CXXFLAGS := -std=c++23 -Wall -Wextra -pedantic\
	-I../../modules/tracy/public

LDLIBS :=

ifeq ($(OS),Windows_NT)
    LDLIBS += -lws2_32 -ldbghelp -lsecur32
endif

TARGET := bin/quadtree_search
TRACY_OBJ := bin/TracyClient.o

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
profile: $(TRACY_OBJ)
profile: SRC += $(TRACY_OBJ)
profile: $(TARGET)

$(TRACY_OBJ):
	$(CXX) -O3 -g -DTRACY_ENABLE -I../../modules/tracy/public -w -c $(TRACY_SRC) -o $(TRACY_OBJ)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET) ../../data/example_data.csv

clean:
	rm -f $(TARGET)