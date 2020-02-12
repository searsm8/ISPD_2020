//Slicing_Annealer.cpp
//author: Mark Sears
//
//Slicing_Annealer take a group of blocks or similar 
//rectangles and performs Simulated Annealer heuristic
//to produce a layout
//

#include "Slicing_Annealer.h"
#include "../kernel/Kernels.h"

//TESTBENCH for Slicing_Annealer

int main()
{
	Kernel* k = new Dblock(128, 32, 32, 1,1,1,1,1,1,2,1);
	k->computePerformance();
	k->printParameters();
	k->printPerformance();
	
	Kernel* k2 = new Dblock(128, 16, 64, 1,1,1,1,1,1,1,1);
	k2->computePerformance();
	k2->printParameters();
	k2->printPerformance();

	Block_Wrapper<Kernel> wrapper1(k);
	Block_Wrapper<Kernel> wrapper2(k2);
	Block_Wrapper<Kernel> wrapperX(&wrapper1, &wrapper2, 'v');

	
	cout << "Block Wrapper1) width: " << wrapper1.getWidth() << " height: " << wrapper1.getHeight() << endl;
	cout << "Block Wrapper2) width: " << wrapper2.getWidth() << " height: " << wrapper2.getHeight() << endl;
	cout << "Block WrapperX) width: " << wrapperX.getWidth() << " height: " << wrapperX.getHeight() << endl;

	return 1;
}
