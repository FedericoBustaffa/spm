CXX 		= g++
CXXFLAGS	+= -Wall -std=c++17
OPTFLAGS	= -O2
INCLUDES	= -I../include
LIBS		=
SOURCES		= $(wildcard *.cpp)
TARGET		= $(SOURCES:.cpp=)

.PHONY: all clean recompile

%: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@.out $< $(LIBS)


all: $(TARGET)

clean:
	-rm -rf *.o *~ *.out *.txt

recompile: clean all
