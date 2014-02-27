# Auto-generated makefile. Modify this as you need. 
# !!! Remember to use the CXX prefix to reference the "ipcp" compiler
#     instead of "g++"
#

# Use Intel C++ Compiler
CXX=g++

# Compiler and Linker flags
CXXFLAGS=
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

core: 
	$(CXX) $(CXXFLAGS) -o $(BINF) $(OBJS) $(LXXFLAGS) $(LIBS)
	rm -rf *o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -c $<

clean:
	rm -rf *o $(BINF)
