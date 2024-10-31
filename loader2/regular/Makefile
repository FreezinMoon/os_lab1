CXX = g++
CXXFLAGS = -Wall
TARGET = loader2_regular

.PHONY: all clean optimized nonoptimized

all: optimized nonoptimized

optimized:
 $(CXX) $(CXXFLAGS) -O2 main.cpp -o $(TARGET)_opt

nonoptimized:
 $(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)_nonopt

clean:
 rm -f $(TARGET)_opt $(TARGET)_nonopt