CXX = g++
CXXFLAGS = -g -Wall -std=c++17 -O2
LIBS = -pthread
TARGET = threading.out

all: $(TARGET)

threading.out: main.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

main.o: main.cpp buffer.hpp
	$(CXX) $(CXXFLAGS) main.cpp -c

clean:
	-rm *.o *.out

recompile: clean all
