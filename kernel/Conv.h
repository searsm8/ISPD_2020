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

	Conv(int H, int W, int R, int S, int C, int K, int T, int h, int w, int c, int k)
	{
		//initialize all Formal Parameters
/*		FP.insert(pair<string, int>("H", H));
		FP.insert(pair<string, int>("W", W));
		FP.insert(pair<string, int>("R", R));
		FP.insert(pair<string, int>("S", S));
		FP.insert(pair<string, int>("C", C));
		FP.insert(pair<string, int>("K", K));
		FP.insert(pair<string, int>("T", T));

		//initialize all Execution parameters
		EP.insert(pair<string, int>("h", h));
		EP.insert(pair<string, int>("w", w));
		EP.insert(pair<string, int>("c", c));
		EP.insert(pair<string, int>("k", k));
	*/
		FP["H"] = H;
		FP["W"] = W;
		FP["R"] = R;
		FP["S"] = S;
		FP["C"] = C;
		FP["K"] = K;
		FP["T"] = T;

		EP["h"] = h;	
		EP["w"] = w;	
		EP["c"] = c;	
		EP["k"] = k;	
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

	string getType() { return "Conv"; }

	//function to update all performance metrics
	void computePerformance()
	{
		computeHeight();
		computeWidth();
		computeTime();
		computeMemory();
	}

//MODIFIERS
	void setX(int new_X) { x = new_X; }
	
	void setY(int new_Y) { y = new_Y; }

	void setEP(string key, int val)
	{
		EP[key] = val;
	}

	//gives an array of ints to help graphically represent the kernel
	//each row in the returned array is one rectangle
	//of the format: [x, y, width, height]
	vector<vector<int>> getRectangles()
	{
		vector<vector<int>> rects;
		rects.push_back(vector<int>{ x, y, width, height});
		return rects;
	}
};
#endif
