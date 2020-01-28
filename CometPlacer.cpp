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
	
	string kgraph;
	string output;
	int wirepenalty;
	int timelimit;
	int width;
	int height;

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

	placer.printInfo();
	placer.updateVisual();
	placer.modifyKernel(0, "h", 2);
	placer.updateVisual();
	placer.modifyKernel(0, "k1", 32);
	placer.updateVisual();
	placer.modifyKernel(0, "k2", 32);
	placer.updateVisual();
	placer.modifyKernel(0, "k3", 32);
	placer.updateVisual();

	return 1;
}
