CXX 			= g++
CXXFLAGS		= -std=c++17
OPTFLAGS		= -O2
AUTOFLAGS		= -fopt-info-vec-missed -O3 -march=native -ffast-math
AVXFLAGS		= -O3 -march=native -mavx
CXXFLAGS		+= -Wall 
INCLUDES		= -I. -I./include
LIBS			= #-pthread -fopenmp
SOURCES			= $(wildcard *.cpp)
TARGET			= $(SOURCES:.cpp=)

.PHONY: all clean cleanall 

%: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o $@.out $< $(LIBS)

# For files with _plain in their name, append flags to CXXFLAGS
%_plain: CXXFLAGS += ${OPTFLAGS}

# For files with _avx in their name, append flags to CXXFLAGS
%_avx: CXXFLAGS += ${AVXFLAGS}

# For files with _auto in their name, append flags to CXXFLAGS
%_auto: CXXFLAGS += ${AUTOFLAGS}

all: $(TARGET)

clean: 
	-rm -fr *.o *~

cleanall: clean
	-rm -fr *.out
