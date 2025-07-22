# Compiler settings
CXX = g++                          # Changed from CC to CXX
CXXFLAGS = -Wall -Wextra -g -I./include  # Changed from CFLAGS to CXXFLAGS
LDFLAGS =

# Project name
TARGET = vps_srv

# Source files
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)      # Changed to .cpp
OBJS = $(SRCS:.cpp=.o)                   # Changed to .cpp

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)  # Changed to CXX

%.o: %.cpp                              # Changed to .cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

publish:
	make clean && git add . && git commit -m "CORE" && git push -u origin main