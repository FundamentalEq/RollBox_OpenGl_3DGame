all: rollbox

rollbox: rollboxcode.cpp
	# g++ -g -o rollbox rollboxcode.cpp -lglfw -lGLEW -lSOIL -lGL -ldl -std=c++11 -I/usr/local/include -std=c++11
		g++ -g -o rollbox rollboxcode.cpp glad.c -lGL -lglfw -ldl -lao -lpthread -lSOIL -I/usr/include -I/usr/local/include -I/usr/local/include/freetype2 -I./include -std=c++11

clean:
	rm rollbox
