# Define the compiler
CXX = g++

# Define the flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Define the target executables
TARGETS = encode decode

# Default rule to build all targets
all: $(TARGETS)

# Rule to build encode executable
encode: encode.o
	$(CXX) $(CXXFLAGS) -o encode encode.o

# Rule to build decode executable
decode: decode.o
	$(CXX) $(CXXFLAGS) -o decode decode.o

# Rule to compile encode.cpp
encode.o: encode.cpp huffman.hpp header.hpp
	$(CXX) $(CXXFLAGS) -c encode.cpp

# Rule to compile decode.cpp
decode.o: decode.cpp huffman.hpp header.hpp
	$(CXX) $(CXXFLAGS) -c decode.cpp

# Clean up the build files
clean:
	rm -f *.o $(TARGETS)

