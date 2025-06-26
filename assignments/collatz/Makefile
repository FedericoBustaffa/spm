# compiler
CXX = g++

# general flags
CXXFLAGS = -Wall -std=c++20

# flags for debug compilation - enabled if run "make -DBUILD_TYPE=DEBUG"
DBGFLAGS = -g

# flags for optimized compilation - disabled if compiled in debug mode
OPTFLAGS = -O3 -march=native

# dependencies flags
DEPSFLAGS = -MMD -MP

# specify include directories with -I<dir>
INCLUDES = -I./include/ -I../../lib/include/

# specify preprocessor definitions
DEFINES = 

# convenient single variable to wrap all the flags
FLAGS = $(CXXFLAGS)
FLAGS += $(INCLUDES)
FLAGS += $(DEFINES)
FLAGS += $(DEPSFLAGS)

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
LIBS = -pthread

# specify source directory
SOURCE_DIR = ./src
SOURCES = $(wildcard $(SOURCE_DIR)/*.cpp)

# build directory containing .o and .d files
BUILD_DIR = build

# directory for .d files
DEPS = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SOURCES))

# generate the object files based on the sources names
OBJECTS = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all clean-fast clean recompile

TARGET = collatz.out

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) $(FLAGS) -c $< -o $@

-include $(DEPS)

clean:
	-rm -rf $(BUILD_DIR)

cleanall: clean
	-rm -rf $(TARGET)

recompile: cleanall all
