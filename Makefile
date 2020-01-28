all:
	g++ CometPlacer.h -std=c++11
	g++ CometPlacer.cpp $(shell pkg-config --cflags --libs sdl2) -std=c++11

clean:
	rm ./a.out CometPlacer.h.gch

run:
	./a.out kgraph=kgraph output=output wirepenalty=100 timelimit=60 width=633 height=633
