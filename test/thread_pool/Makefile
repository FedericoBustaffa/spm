CXX = g++
CXXFLAGS += -Wall -std=c++17
OPTFLAGS = -O3 -march=native
LIBS = -pthread
TARGETS = submit.out map.out

.PHONY: all clean recompile 

%.out: %.cpp task_queue.hpp thread_pool.hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

all: $(TARGETS)

clean: 
	-rm -fr *.o *.out

recompile: clean all
