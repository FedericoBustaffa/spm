CXX = g++
OPTFLAGS = -O3 -march=native
CXXFLAGS += -Wall -std=c++17
LIBS = -pthread
SOURCES = $(wildcard *.cpp)
TARGET = $(SOURCES:.cpp=.out)

.PHONY: all clean recompile 

%.out: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

all: $(TARGET)

clean: 
	-rm -fr *.o *.out

recompile: clean all
