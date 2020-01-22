//test_kernels.cpp
//
//

#include "Kernel.h"
#include "Conv.h"
#include <vector>

int main()
{
	Conv k0 = Conv(28, 28, 1, 1, 256, 256/4, 1);
	Kernel k1;
	
	k0.printParams();
	k1.printParams();
	
	k0.printPerformance();
	k1.printPerformance();
	

	vector<Kernel> myKernels;
//	myKernels.push_back(k0);
	
//	myKernels[0].printParams();
}
