# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -g -I./include
LDFLAGS = 

# Project name
TARGET = vps_srv

# Source files
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

publish:
	make clean && git add . && git commit -m "CORE" && git push -u origin main