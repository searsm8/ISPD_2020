all:
	g++ -Wall -g CometPlacer.cpp $(shell pkg-config --cflags --libs sdl2) -std=c++11 -o CometPlacer.out

clean:
	rm CometPlacer.out placement.paint

run:	
	./CometPlacer.out < benchmarks/kgraph.txt
run2:	
	./CometPlacer.out < benchmarks/kgraph2.txt
run3:	
	./CometPlacer.out < benchmarks/kgraph3.txt
run4:	
	./CometPlacer.out < benchmarks/kgraph4.txt
run_single:	
	./CometPlacer.out < benchmarks/single.txt
