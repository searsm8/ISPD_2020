//Dblock.h
//author: searsm8
//
//extends Kernel
//represents a dblock
//

#ifndef _Dblock
#define _Dblock

#include "Kernel.h"
#include "Conv.h"

class Dblock : public Kernel
{
private:
public:
	//each Dblock holds 3 Convolutional Kernels
	Conv conv1;
	Conv conv2;
	Conv conv3;

	//constructors
	Dblock()
	{}

	Dblock(int H, int W, int F)
	{
		//initialize 3 internal convolutional kernels
		conv1 = Conv(H, W, 1, 1, F, F/4, 1);
		conv2 = Conv(H, W, 3, 3, F/4, F/4, 1);
		conv3 = Conv(H, W, 1, 1, F/4, F, 1);
		
		//initialize formal parameters
		FP.insert(pair<string, int>("H", H));	
		FP.insert(pair<string, int>("W", W));	
		FP.insert(pair<string, int>("F", F));	

		//initialize Execution paramaters
		EP.insert(pair<string, int>("h", 1));	
		EP.insert(pair<string, int>("w", 1));	
		EP.insert(pair<string, int>("c1", 1));	
		EP.insert(pair<string, int>("k1", 1));	
		EP.insert(pair<string, int>("c2", 1));	
		EP.insert(pair<string, int>("k2", 1));	
		EP.insert(pair<string, int>("c3", 1));	
		EP.insert(pair<string, int>("k3", 1));	
		
	}


};
#endif
