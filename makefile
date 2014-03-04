# Auto-generated makefile. Modify this as you need. 
# !!! Remember to use the CXX prefix to reference the "icpc" compiler
#     instead of "g++"
#
# !!! This makefile assumes no special dependencies and will attempt to 
#     compile all local .cpp files into corresponding .o's and then a 
#     single binary of the same name as the repository...

# Use Intel C++ Compiler
CXX=icpc

# Compiler and Linker flags
CXXFLAGS= -Wall -fast -openmp -mavx 
LXXFLAGS= 

# Inlude and Library paths
INCLUDES=
LIBS= 

# Inputs (grab all .cpp files, and assume change to .o)
SOURCES := $(wildcard *.cpp)
OBJS := $(patsubst %.c,%.o,$(wildcard *.cpp))

# Ouput to binary
BINF=phihash

all: core

core: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BINF) $(OBJS) $(LXXFLAGS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -c $<

clean:
	rm -rf *o $(BINF)
