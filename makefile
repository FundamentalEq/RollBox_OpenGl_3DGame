all: sample2D

sample2D: ass2.cpp
	# g++ -g -o sample2D ass2.cpp -lglfw -lGLEW -lSOIL -lGL -ldl -std=c++11 -I/usr/local/include -std=c++11
	g++ -g -o sample2D ass2.cpp glad.c  -ldl -lGL -lglfw  -lSOIL -I/usr/local/include -I/usr/local/include/freetype2 -L/usr/local/lib -std=c++11

clean:
	rm sample2D
