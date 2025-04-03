CXX 		= g++
CXXFLAGS	+= -Wall -std=c++14 -march=native -mavx -mavx2 -mfma
OPTFLAGS	= -O3
INCLUDES	= -I. -I./include
LIBS		= #-pthread
SOURCES		= $(wildcard *.cpp)
TARGET		= $(SOURCES:.cpp=)

.PHONY: all clean recompile

%: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@.out $< $(LIBS)


all: $(TARGET)

clean:
	-rm -rf *.o *~ *.out *.txt

recompile: clean all
