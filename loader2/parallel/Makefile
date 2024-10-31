CXX = g++
CXXFLAGS = -Wall -fopenmp
TARGET = loader2_parallel

.PHONY: all clean

all:
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET)