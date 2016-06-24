.PHONY: all clean

CXX = g++
CXX_FLAGS += -O3

all: test train

test: test.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

train: train.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

clean:
	rm train test
