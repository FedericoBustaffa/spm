CXX                = nvcc -arch=sm_35 -ccbin /usr/bin/g++-10 -Wno-deprecated-gpu-targets -std=c++17 -x cu
OPTFLAGS	   = -O3
CXXFLAGS          += 
INCLUDES	   = -I . 
SOURCES            = $(wildcard *.cu)
TARGET             = $(SOURCES:.cu=)

.PHONY: all clean cleanall 

%: %.cu
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@.out $< $(LIBS)

# For files with _avx in their name
%_avx: CXX = g++ -std=c++17
%_avx: CXXFLAGS += -march=native


all: $(TARGET)

clean: 
	-rm -fr *.o

cleanall: clean
	-rm -fr *.out
