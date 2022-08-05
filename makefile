all: compile

compile: mandelbrot.cpp
	g++ -lncurses mandelbrot.cpp -o mandelbrot
