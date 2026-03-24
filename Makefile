CXX := g++

COMMON_FLAGS := -std=c++17 -O3 -g -Wall -Wextra -pedantic -march=native
INCLUDES := -Iinclude

SRC := \
	src/main.cpp \
	src/utf8.cpp \
	src/input.cpp \
	src/grades.cpp \
	src/fileio.cpp \
	src/sorting.cpp \
	src/table.cpp \
	src/menu.cpp \
	src/fileGenerator.cpp \
	src/splitting.cpp \
	src/benchmark.cpp

TARGETS := vector list deque

all: $(TARGETS)

vector: CXXFLAGS := $(COMMON_FLAGS) -DSTUDENT_CONTAINER_VECTOR
vector: $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@

list: CXXFLAGS := $(COMMON_FLAGS) -DSTUDENT_CONTAINER_LIST
list: $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@

deque: CXXFLAGS := $(COMMON_FLAGS) -DSTUDENT_CONTAINER_DEQUE
deque: $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@

clean:
	rm -f $(TARGETS) benchmark_*.csv kietiakai.txt vargsiukai.txt

.PHONY: all clean
