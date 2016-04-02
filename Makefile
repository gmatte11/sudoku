.PHONY=all clean

CXX=clang++
CXXFLAGS=-std=c++1z -g -Iinclude -Irange-v3/include
LD=clang++
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
