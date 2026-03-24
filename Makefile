CXX := g++
CXXFLAGS := -std=c++17 -O3 -g -Wall -Wextra -pedantic -march=native
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

TARGET := vector

all: $(TARGET)

vector: CXXFLAGS := $(COMMON_FLAGS) -DSTUDENT_CONTAINER_VECTOR
vector: $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@

list: CXXFLAGS := $(COMMON_FLAGS) -DSTUDENT_CONTAINER_LIST
list: $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@

clean:
	rm -f $(TARGET)
