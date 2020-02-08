all:
	g++ CometPlacer.h -std=c++11
	g++ CometPlacer.cpp $(shell pkg-config --cflags --libs sdl2) -std=c++11 -o CometPlacer.out

clean:
	rm CometPlacer.out CometPlacer.h.gch

run:
	./CometPlacer.out kgraph=benchmarks/kgraph.txt output=output wirepenalty=100 timelimit=60 width=633 height=633

run_single:
	./CometPlacer.out kgraph=benchmarks/single.txt output=output wirepenalty=100 timelimit=60 width=633 height=633
