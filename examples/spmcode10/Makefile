CXX                = mpicxx -std=c++20
OPTFLAGS	   = -O3 -march=native -ffast-math
CXXFLAGS          += -Wall
INCLUDES	   = -I. -I./include
LIBS               =
SOURCES            = $(wildcard *.cpp)
TARGET             = $(SOURCES:.cpp=.out)

.PHONY: all clean cleanall

%.out: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

all: $(TARGET)

clean:
	-rm -fr *.o *~
cleanall: clean
	-rm -fr $(TARGET)
