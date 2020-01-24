//Conv.h
//represents a Convolutional kernel

#ifndef _Conv
#define _Conv

#include "Kernel.h"


class Conv : public Kernel
{
public:
	//constructors
	//
	Conv()
	{}

	Conv(int H, int W, int R, int S, int C, int K, int T)
	{
		//initialize all Formal Parameters
		FP.insert(pair<string, int>("H", H));
		FP.insert(pair<string, int>("W", W));
		FP.insert(pair<string, int>("R", R));
		FP.insert(pair<string, int>("S", S));
		FP.insert(pair<string, int>("C", C));
		FP.insert(pair<string, int>("K", K));
		FP.insert(pair<string, int>("T", T));

		//initialize all Execution parameters
		EP.insert(pair<string, int>("h", 1));
		EP.insert(pair<string, int>("w", 1));
		EP.insert(pair<string, int>("c", 1));
		EP.insert(pair<string, int>("k", 1));
	}

	int computeHeight()
	{
		height = EP["h"] * EP["w"] * (EP["k"]+1);
		return height;
	}

	int computeWidth()
	{
		width = 3 * EP["c"];
		return width;
	}

	int computeTime()
	{
		time = ceil(FP["H"]/EP["h"]) * ceil(FP["W"]/EP["w"]) * ceil(FP["C"]/EP["c"]) * ceil(FP["K"]/EP["k"]) * FP["R"] * FP["S"] / FP["T"] / FP["T"];

		return time;
	}

	int computeMemory()
	{
		memory = FP["C"]/EP["c"] * FP["K"]/EP["k"] * FP["R"] * FP["S"] +
			(FP["W"]+FP["S"]-1)/EP["w"] * (FP["H"]+FP["R"]-1)/EP["h"] * FP["K"]/EP["k"];
		return memory;
	}
};
#endif
