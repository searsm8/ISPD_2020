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
	placer.printTimeAndArea();
	placer.printKernels();

	//placer.printKernels();
	//placer.printInfo();
	cout << "avg_time: " << placer.computeAvgTime() << endl;
	cout << "max_time: " << placer.getMaxTime() << endl;
	placer.updateVisual();

//	placer.equalizeKernelTimes();
	cout << "avg_time: " << placer.computeAvgTime() << endl;
	cout << "max_time: " << placer.getMaxTime() << endl;
	placer.updateVisual(); 
	
	placer.maximizeKernelSize(0.8);
	placer.enforceMemoryConstraint();
	//placer.printKernels();
	placer.achievePreciseAR(0.01);
	cout << "avg_time: " << placer.computeAvgTime() << endl;
	cout << "max_time: " << placer.getMaxTime() << endl;
	placer.updateVisual(); 
	placer.printARs();
	return 1;
}
