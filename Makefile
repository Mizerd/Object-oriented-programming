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
	src/menu.cpp

TARGET := vector

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
