CXX                = g++
OPTFLAGS	   = -O3
AUTOFLAGS          = -march=native -ffast-math -mavx2
CXXFLAGS          += -Wall -std=c++17
INCLUDES	   = -I. -I./include
LIBS               = #-pthread
SOURCES            = $(wildcard *.cpp)
TARGET             = $(SOURCES:.cpp=.out)

.PHONY: all clean recompile

%.out: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

# For files with _avx in their name, append -mavx2 -mfma to CXXFLAGS
%_avx.out: CXXFLAGS += -mavx2 -mfma

all: $(TARGET)

clean: 
	-rm -fr *.o *.out

recompile: clean all

