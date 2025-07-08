# compiler
CXX = g++

# general flags
CXXFLAGS = -Wall -std=c++20

# flags for debug compilation - enabled if run "make -DBUILD_TYPE=DEBUG"
DBGFLAGS = -g

# flags for optimized compilation - disabled if compiled in debug mode
OPTFLAGS = -O3 -march=native

# specify include directories with -I<dir>
INCLUDES = -I./lib/include/ -I../lib/include/

# specify preprocessor definitions
DEFINES =

# convenient single variable to wrap all the flags
FLAGS = $(CXXFLAGS)
FLAGS += $(INCLUDES)
FLAGS += $(DEFINES)

# default compilation is with opt flags
BUILD_TYPE ?= RELEASE
ifeq ($(BUILD_TYPE),DEBUG)
	FLAGS += $(DBGFLAGS)
else ifeq ($(BUILD_TYPE),RELEASE)
	FLAGS += $(OPTFLAGS)
else
	$(error "Invalid BUILD_TYPE. Use DEBUG or RELEASE.")
endif

# link libraries
LIBS = -pthread -L./lib/build/ -lmergesort

.PHONY: all clean recompile

TARGETS = $(patsubst %.cpp, %.out, $(wildcard *.cpp))

all:
	$(MAKE) -C lib
	$(MAKE) $(TARGETS)

%.out: %.cpp ./lib/build/libmergesort.a
	$(CXX) $(FLAGS) $< -o $@ $(LIBS)

clean:
	$(MAKE) -C lib clean
	-rm -rf $(TARGETS)

recompile: clean all

