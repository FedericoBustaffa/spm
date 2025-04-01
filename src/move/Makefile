CXX 		= g++
OPTFLAGS	= -O2
INCLUDES	= -I./include/
CXXFLAGS	+= -Wall -std=c++17 $(OPTFLAGS) $(INCLUDES)
TARGET		= move.out

.PHONY: all clean recompile

all: $(TARGET)

# executable
move.out: main.o buffer.o
	$(CXX) $^ -o $@

# object files
main.o: main.cpp
buffer.o: buffer.hpp buffer.cpp

clean:
	-rm -rf *.o *.out

recompile: clean all
