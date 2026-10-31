PROJECT_NAME = Prototype3

RAYLIB_PATH = C:/raylib/raylib

# Compiler and flags for Windows
# Uses the raylib built-in compiler 
CXX = C:/raylib/w64devkit/bin/g++.exe
# Look through include and src files. 
CXXFLAGS = -Iinclude -Wall -std=c++17 -O1 -I$(RAYLIB_PATH)/src -Isrc

# Linker flags and libraries.
LDFLAGS = -L$(RAYLIB_PATH)/src
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Simply compile all .cpp files in src (source).
SRCS = $(wildcard src/*.cpp)
# Get rid of the src part though.
OBJS = $(notdir $(SRCS:.cpp=.o))

# Default target
all: $(PROJECT_NAME)

# Link the executables.
$(PROJECT_NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(PROJECT_NAME).exe $(LDFLAGS) $(LDLIBS)

# Compiles all .cpp files into .o
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	del *.o $(PROJECT_NAME).exe