# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++11 -Wall -Wextra

# Include directories
INCLUDES := -I/usr/include/mysql

# Library directories
LIBDIRS := -L/usr/lib/mysql

# Libraries
LIBS := -lmysqlclient -lssl -lcrypto -lboost_system -lboost_thread 

# Source files
SRCS := $(wildcard *.cpp)

# Executable names
EXECUTABLES := $(SRCS:.cpp=)

# Default target
all: $(EXECUTABLES)

# Clean rule
clean:
	rm -f $(EXECUTABLES)

# Rule to make individual executables
%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBDIRS) -o $@ $< $(LIBS)
