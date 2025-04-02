CXX = nvcc -arch=sm_35 -ccbin /usr/bin/g++-10 -Wno-deprecated-gpu-targets
OPTFLAGS = -O3
CXXFLAGS += -std=c++17 -x cu
INCLUDES = -I . 
SOURCES = $(wildcard *.cu)
SOURCES += $(wildcard *.cpp)
TARGET = $(SOURCES:.cu=.out)
TARGET += $(SOURCES:.cpp=.out)

.PHONY: all clean recompile 

all: $(TARGET)

%.out: %.cu
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $^ -o $@ $(LIBS)

conv1D_avx.out: conv1D_avx.cpp
	g++ -std=c++17 -O3 -march=native $(INCLUDES) $^ -o $@ $(LIBS)

clean: 
	-rm -fr *.o *.out

recompile: clean all

