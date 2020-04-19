all:
	g++ -Wall -g CometPlacer.cpp $(shell pkg-config --cflags --libs sdl2) -std=c++11 -o CometPlacer.out

clean:
	rm CometPlacer.out placement.paint .*swp

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
runfork:	
	./CometPlacer.out < benchmarks/fork.txt
runconv:	
	./CometPlacer.out < benchmarks/conv.txt
runa:
	./CometPlacer.out < benchmarks/A.kgraph
runb:
	./CometPlacer.out < benchmarks/B.kgraph
runc:
	./CometPlacer.out < benchmarks/C.kgraph
rund:
	./CometPlacer.out < benchmarks/D.kgraph
rune:
	./CometPlacer.out < benchmarks/E.kgraph
runf:
	./CometPlacer.out < benchmarks/F.kgraph
rung:
	./CometPlacer.out < benchmarks/G.kgraph
runh:
	./CometPlacer.out < benchmarks/H.kgraph
