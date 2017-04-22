.PHONY=all clean

CXX=g++
CXXFLAGS=-std=c++17 -fconcepts -g -Iinclude -Irange-v3/include
LD=g++
LDFLAGS=
LDLIBS=

SRCS=main.cpp
OBJS=$(patsubst %.cpp,build/%.o,$(SRCS))
TARGET=sudoku

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf build
	rm -f $(TARGET)

build:
	mkdir build
