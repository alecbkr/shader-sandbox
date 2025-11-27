CXX = clang++
CC = clang


# DETECT OS
ifeq ($(OS),Windows_NT)
  EXLINKS = ./lib/glfw/libglfw3dll.a -lopengl32 -lgdi32 -luser32 -lshell32 -lwinmm
  TARGET  = ./bin/build.exe
  RM      = del /Q
  CLEAN_OBJ = obj\*.o obj\*.d
  CLEAN_BIN = bin\*.exe
  NULL = nul
  SHELL   = cmd.exe   # ensures 'del' works
else 
  UNAME_S := $(shell uname -s 2>/dev/null)
  
  # MAC
  ifeq ($(UNAME_S),Darwin)
    EXLINKS = ./lib/libglfw.3.dylib -framework OpenGL -framework Cocoa -framework IOKit
    TARGET  = ./bin/build
    RM      = rm -f
	CLEAN_OBJ = ./obj/*.o ./obj/*.d
  	CLEAN_BIN = ./bin/*
  	NULL = /dev/null
  endif
endif


CXXFLAGS =
CFLAGS = 
LDFLAGS = 

# Basic Setup (Compile with all warnings and c++17 in mind)
CXXFLAGS += -Wall -std=c++17 -MMD -MP \
		   -Iinclude 

CFLAGS += -Wall -std=c17 -MMD -MP \
		  -Iinclude 
		   
		   
# GLFW | GLM 
CXXFLAGS += -I"./include/glm" 
CXXFLAGS += -I"./include/glad"
LDFLAGS += -L"./lib"

# IMGUI
CXXFLAGS += -I"./include/imgui"



# CPP/C SRC FILES
SRC_CPP = $(wildcard ./src/*.cpp)		  # main.cpp
SRC_CPP += $(wildcard ./src/engine/*.cpp) # engine folder
SRC_CPP += $(wildcard ./src/imgui/*.cpp)  # imgui folder

SRC_C = ./lib/glad.c 					  # glad.c

# Object files are routed to obj directory
OBJ_CPP = $(patsubst ./src/%.cpp, ./obj/%.o, $(SRC_CPP))
OBJ_C = $(patsubst ./lib/%.c, ./obj/%.o, $(SRC_C))
OBJ = $(OBJ_CPP) $(OBJ_C) 

# Get Directories and prep
# OBJ_DIRS = $(sort $(dir $(OBJ)))

# prepare_dirs:
# 	@for %%d in ($(OBJ_DIRS)) do if not exist %%d mkdir %%d



# Adding .d files to track hpp files
DEPS = $(OBJ:.o=.d)

.PHONY: all clean

# Main rule which depends on target
# all: prepare_dirs $(TARGET)
all: $(TARGET)

# Built-in Make Variables:
# $@ -> Rule target
# $^ -> All prereqs
# $< -> First prereq

# Target rule depends on all the object files
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(EXLINKS)

# Object file rule to build .o files from .cpp
# Also ensures obj directory exists
./obj/%.o: ./src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

./obj/%.o: ./src/engine/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

./obj/%.o: ./src/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

./obj/%.o: ./lib/glad/%.c
	$(CC) $(CFLAGS) -c $< -o $@


./obj/%.o: ./lib/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-$(RM) $(CLEAN_OBJ) 2>$(NULL)
	-$(RM) $(CLEAN_BIN) 2>$(NULL)

-include $(DEPS)