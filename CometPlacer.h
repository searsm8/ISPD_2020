//CometPlacer.h
//author: Mark Sears
//
//Defines CometPlacer class, which manipulates kernels 
//in the WSE to optimize performance
#ifndef _CometPlacer
#define _CometPlacer

#include "util.h"
#include "Xblock.h"
#include "Slicing_Annealer.h"
#include <cstdio>

class CometPlacer
{
private:
	int wirepenalty; //how much to weight the wirelength
	int timelimit; //runtime limit for program execution
	int width;
	int height;

	//keep pointers to all the kernels in the layout
	vector<Kernel*> kernels;


	Kernel* head; //head of the chain of kernels
	int iteration;
	long avg_time;

	string output;
public:
	Slicing_Annealer<Kernel> annealer;

#ifdef VISUALIZE
	SDL_Window* window;
#endif


	//constructor
	CometPlacer();
	CometPlacer(string kgraph_filename, string output, int wirepenalty, int timelimit, int width, int height);
	

	void readParameter(string line);
	void readNode(string line);
	void readConnection(string line);
	
	//read a specified input file for kernel information
	void readKgraph(string kgraph_filepath);

//PRINT METHODS

	void printPercentFilled();

	void fixName(Kernel* k, int ID=0);

	//print the current best solution to file
	//specifying block placements and parameters
	void printOutputToFile(string output_filepath);
	void printUnion(Kernel* k, ofstream& output_file);
	void printKernelToFile(Kernel* k, ofstream& output_file);
	
	//print general info about the WSE layout
	void printInfo();

	//print the kernel parameters and placements
	void printKernels();
	
	void printTimeAndArea();
	
	//visualize the kernels on the WSE
	void updateVisual(bool wait_for_anykey=false, int epoch_count=0);

	void resetTimestamp();
	void printTimestamp();


//MODIFIERS

	//compute the L1 Penalty for the whole dam thing
	int computeL1Penalty();
	
//ACCESSORS
	int computeTotalKernelArea();
	int getWaferArea() { return width*height; }

	//give each kernel an intial placement
	//place all the kernels spread evenly across the wafer in a grid
	void setInitialPlacement();
	
	long long computeTotalTime();
	
	int computeAvgTime();
	
	int getLongestTime();
	int getShortestTime();
	int getAverageTime();
	
	//return pointer to the kernel with the longest execution time
	Kernel* getLongestKernel();
	Kernel* getShortestKernel();
	
	void increaseAllEP(string key, int increment=1);
	
	void inflateKernelSize(double fill_percent_p1 = 0.75);
	
	//check that each kernel meets the memory constraint
	//and does not use more then 48kB per core
	//if a kernel requires too much memory,
	//increase the size
	bool enforceMemoryConstraint();
		
	void decreaseBlocks();

	//use Slicing_Annealer object to perform Simulated annealing on the kernels
	void performAnnealing();
	
	void computePossibleKernels();

	bool legalizeLayout();
	void finalizeLayout();
};
#endif
