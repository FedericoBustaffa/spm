CXX                = g++ -std=c++17
OPTFLAGS	   = -O3 -march=native
CXXFLAGS          += -Wall 
INCLUDES	   = -I. -I./include
LIBS               = -pthread
SOURCES            = $(wildcard *.cpp)
TARGET             = $(SOURCES:.cpp=.out)

.PHONY: all clean recompile

%.out: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

all: $(TARGET)

some-cpp-features: some-cpp-features.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $<

clean: 
	-rm -fr *.o *.out

recompile: clean all

