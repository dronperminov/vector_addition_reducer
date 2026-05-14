CXX = g++
FLAGS = -Wall -O3 -std=c++17 -fopenmp
OBJECTS = src/entities/arg_parser.o src/entities/vector.o src/vector_reducer.o

all: vector_addition_reducer

vector_addition_reducer: $(OBJECTS)
	$(CXX) $(FLAGS) $(OBJECTS) vector_addition_reducer.cpp -o vector_addition_reducer

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS) vector_addition_reducer
