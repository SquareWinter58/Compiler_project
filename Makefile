
# print: $(wildcard *.c)
# 	ls -la $?

# $@ Targe name
# $? all prerequisites newer than the target
# $^ all prerequisites
# $< first prerequisite

# funciton $(patsubst %.c,%.0,$(wildcard *.c))
# the following will change the list of c source files in a directory to a list with the suffix .o

# The following rule takes advantage of impilicit rules and the function mentioned above to build an executable
# from all the c files in the directory, after first compiling them to object files

# objects := $(patsubst %.c,%.o,$(wildcard *.c))

# foo: $(objects)
# 	cc -o foo $(objects)
VPATH = src:include
SOURCES=$(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,bin/%.o,$(SOURCES))
CXX=g++
CXXFLAGS=-std=c++20 -g -O0 $(patsubst %,-I%,$(subst :, ,$(VPATH)))
#OBJECTS=file1.o file2.o file3.o
EXECUTABLE=lexer

all: $(EXECUTABLE) $(ARTIFICIAL_DEP)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	echo -e "CompileFlags:\n\tAdd: [--include-directory=$$(pwd)/include, -Wall]" > .clangd


bin/%.o: src/%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@



.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)