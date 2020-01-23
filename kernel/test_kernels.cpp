//test_kernels.cpp
//
//

#include "Kernel.h"
#include "Conv.h"
#include "Dblock.h"
#include <vector>

int main()
{
	Conv k0 = Conv(28, 28, 1, 1, 256, 256/4, 1);
	Conv k1 = Conv(48, 48, 1, 1, 256, 256/4, 1);
	Conv k2 = Conv(88, 88, 1, 1, 256, 256/4, 1);
	Dblock d0 = Dblock(48, 48, 1);
	
	d0.x = 200;
	d0.y = 300;
	d0.conv1.x = 200;
	d0.conv1.y = 300;
	d0.conv2.x = 400;
	d0.conv2.y = 300;

	k0.x = 1200;

	k0.computeHeight();
	k0.computeWidth();
	k0.computeTime();
	k0.computeMemory();
//	d0.conv1.printParameters();
	k0.printParameters();
//	k1.printParameters();
//	k2.printParameters();
//	d0.printParameters();

	k0.printPerformance();	

	vector<Kernel> myKernels;
//	myKernels.push_back(k0);
	
//	myKernels[0].printParams();
}
