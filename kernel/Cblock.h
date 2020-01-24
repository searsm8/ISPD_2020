//Cblock.h
//author: searsm8
//
//extends Kernel
//represents a cblock
//

#ifndef _Cblock
#define _Cblock

#include "Kernel.h"
#include "Conv.h"
#include <algorithm>

using namespace std;

class Cblock : public Kernel
{
private:
public:
	//each Dblock holds 3 Convolutional Kernels
	Conv conv1;
	Conv conv2;
	Conv conv3;
	Conv conv4;

	//constructors
	Cblock()
	{}

	Cblock(int H, int W, int F)
	{
		//initialize 3 internal convolutional kernels
		conv1 = Conv(H, W, 1, 1, F, F/4, 1);
		conv2 = Conv(H, W, 3, 3, F/4, F/4, 2);
		conv3 = Conv(H, W, 1, 1, F/4, F, 1);
		conv4 = Conv(H, W, 1, 1, F/2, F, 2);
		
		//initialize formal parameters
		FP.insert(pair<string, int>("H", H));	
		FP.insert(pair<string, int>("W", W));	
		FP.insert(pair<string, int>("F", F));	

		//initialize Execution paramaters
		//not needed???
		EP.insert(pair<string, int>("h", 1));	
		EP.insert(pair<string, int>("w", 1));	
		EP.insert(pair<string, int>("c1", 1));	
		EP.insert(pair<string, int>("k1", 1));	
		EP.insert(pair<string, int>("c2", 1));	
		EP.insert(pair<string, int>("k2", 1));	
		EP.insert(pair<string, int>("c3", 1));	
		EP.insert(pair<string, int>("k3", 1));	
		EP.insert(pair<string, int>("c4", 1));	
		EP.insert(pair<string, int>("k4", 1));	
	
	}

	//take in a new set of Execution Parameters	
	void setEP(int h, int w, int c1, int k1, int c2, int k2, int c3, int k3, int c4, int k4)
	{
		conv1.EP["h"] = h;
		conv1.EP["w"] = w;
		conv2.EP["h"] = h;
		conv2.EP["w"] = w;
		conv3.EP["h"] = h;
		conv3.EP["w"] = w;
		conv4.EP["h"] = h;
		conv4.EP["w"] = w;

		conv1.EP["c"] = c1;
		conv1.EP["k"] = k1;
		conv2.EP["c"] = c2;
		conv2.EP["k"] = k2;
		conv3.EP["c"] = c3;
		conv3.EP["k"] = k3;
		conv4.EP["c"] = c4;
		conv4.EP["k"] = k4;
	}
	int computeHeight()
	{
		height = max(max(conv1.computeHeight(), conv2.computeHeight()), max(conv3.computeHeight(), conv4.computeHeight()));
		return height;
	}

	int computeWidth()
	{
		width = conv1.computeWidth() + conv2.computeWidth() + conv3.computeWidth() + conv4.computeWidth();;
	       return width;	
	}

	int computeTime()
	{
		time = max({conv1.computeTime(), conv2.computeTime(), conv3.computeTime(), conv4.computeTime()});
		return time;
	}
	
	int computeMemory()
	{
		memory = max({conv1.computeMemory(), conv2.computeMemory(), conv3.computeMemory(), conv4.computeMemory()});
		return memory;
	}
	
	//function to update all performance metrics
	void computePerformance()
	{
		computeHeight();
		computeWidth();
		computeTime();
		computeMemory();
	}


};
#endif
