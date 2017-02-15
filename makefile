all: sample2D

sample2D: ass2.cpp
	# g++ -g -o sample2D ass2.cpp -lglfw -lGLEW -lSOIL -lGL -ldl -std=c++11 -I/usr/local/include -std=c++11
		g++ -g -o sample2D ass2.cpp glad.c -lGL -lglfw -ldl -lao -lpthread -lSOIL -I/usr/include -I/usr/local/include -I/usr/local/include/freetype2  -std=c++11

clean:
	rm sample2D
