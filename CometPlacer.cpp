//CometPlacer.cpp
//author: Mark Sears
//
//Primary driver for Wafer Space Enginer placer
//

#include "CometPlacer.h"
#include "util.h"

int main(int argc, char** argv)
{
	cout << "****CometPlacer for WSE****\n\n";
	srand(time(0)); //set random seed
	
	string kgraph;
	string output;
	int wirepenalty;
	int timelimit;
	int width;
	int height;

	//read the runtime arguments
	for(int i = 1; i < argc; i++)
	{
		vector<string> input = split(argv[i], "=");

		if(input.size() > 1)
		{
			if(input[0] == "kgraph")
			       	kgraph = input[1];
			if(input[0] == "output")
				output= input[1];
			if(input[0] == "wirepenalty")
				wirepenalty = stoi(input[1]);
			if(input[0] == "timelimit")
				timelimit= stoi(input[1]);
			if(input[0] == "width")
				width = stoi(input[1]);
			if(input[0] == "height")
				height = stoi(input[1]);
		}
	}

	CometPlacer placer = CometPlacer(kgraph, output, wirepenalty, timelimit, width, height);

	placer.updateVisual();
	//placer.printTimeAndArea();
	//placer.printKernels();

	placer.enforceMemoryConstraint();
	placer.updateVisual(true); 
	placer.inflateKernelSize(0.8);
	placer.updateVisual(true); 

	placer.performAnnealing();

	return 1;
}
