PROJECT_NAME = Prototype

RAYLIB_ROOT = C:/raylib
RAYLIB_PATH = ${RAYLIB_ROOT}/raylib

# Compiler and flags for Windows
# Uses the raylib built-in compiler 
CXX = ${RAYLIB_ROOT}/w64devkit/bin/g++.exe
WINDRES = ${RAYLIB_ROOT}/w64devkit/bin/windres.exe
# Look through include and src files. 
CXXFLAGS = -Iinclude -Wall -std=c++17 -O1 -I$(RAYLIB_PATH)/src -Isrc

# Linker flags and libraries.
LDFLAGS = -L$(RAYLIB_PATH)/src
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -mconsole

# Simply compile all .cpp files in src (source).
SRCS = $(wildcard src/*.cpp)
# Get rid of the src part though.
OBJS = $(notdir $(SRCS:.cpp=.o))
# Also resource file.
RES = resource.res

# --------------------------------------------------------------------

# Default target
all: $(PROJECT_NAME)

# Link the executables.
$(PROJECT_NAME): $(OBJS) ${RES}
	$(CXX) $(OBJS) ${RES} -o $(PROJECT_NAME).exe $(LDFLAGS) $(LDLIBS)

# Compiles all .cpp files into .o
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile the resource file.
${RES}: assets/resource.rc assets/app.ico
	${WINDRES} assets/resource.rc -O coff -o $@

# Clean up build files
clean:
	del *.o *.res $(PROJECT_NAME).exe