all: sample2D

sample2D: ass2.cpp
	g++ -g -o sample2D ass2.cpp -lglfw -lGLEW -lGL -ldl -std=c++11

clean:
	rm sample2D
