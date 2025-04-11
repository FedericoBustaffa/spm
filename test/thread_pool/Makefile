CXX = g++
CXXFLAGS += -Wall -std=c++17
OPTFLAGS = -O3 -march=native
INCLUDES = -I../../include/
LIBS = -pthread
TARGETS = submit.out map.out

.PHONY: all clean recompile 

all: $(TARGETS)

%.out: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

clean: 
	-rm -fr *.o *.out

recompile: clean all
