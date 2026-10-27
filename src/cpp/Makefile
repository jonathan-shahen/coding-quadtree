# Makefile

CXX := g++
# Do not put optimization flags here, instead ONLY add them to ALL. Debug requires NO optimizations for break points
CXXFLAGS := -std=c++23 -Wall -Wextra -pedantic

TARGET := bin/quadtree_search
SRC := src/main.cpp src/quadtree.cpp

.PHONY: all clean run

all: CXXFLAGS += -O2
all: $(TARGET)

debug: CXXFLAGS += -g
debug: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) ../../data/example_data.csv

clean:
	rm -f $(TARGET)