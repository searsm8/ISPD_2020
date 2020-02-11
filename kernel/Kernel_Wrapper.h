//Kernel_Wrapper.h
//
//A Kernel Wrapper is a simple structure which helps organize Kernels for slicing Simulated Annealing
//It contains either 1 Kernel or 2 other Kernel Wrappers
//(Each Kernel might in turn contain other Kernels, such as Dblock or Cblock)
//If the Kernel Wrapper contains a single Kernel, it is a "base level" wrapper
//If it contains 2 other Wrappers, it is somewhere in the hierarchy that makes up the layout
//and has either a V-cut or H-cut seperating the two Kernels
//
//Finally, the top level Kernel_Wrapper will contain all kernels in the layout
//and will *hopefully* fit on the WSE!

#ifndef _KERNEL_WRAPPER
#define _KERNEL_WRAPPER

#include "Kernel.h"

class Kernel_Wrapper
{
private:
	bool base_level;
	char cut; //'h' for horizontal cut, or 'v' for vertical cut

       	//if the Wrapper is base level, then it contains a single Kernel
	Kernel* base_kernel;

	//otherwise, it will contain two other Wrappers
	Kernel_Wrapper* w1;
	Kernel_Wrapper* w2;

	double w_width, w_height; //wrapper width and height

public:
//CONSTRUCTORS
	
	//Kernel_Wrapper() {} //default constructor should not be used?

	Kernel_Wrapper(Kernel* k)
	{
		base_level = true;
		base_kernel = k;
		cut = 'h';

		updateDimensions();
	}

	Kernel_Wrapper(Kernel_Wrapper* a, Kernel_Wrapper* b)
	{
		base_level = true;
		w1 = a;
		w2 = b;
		cut = 'h';

		updateDimensions();
	}

//ACCESSORS

	double getWidth()
	{
		return w_width;
	}

	double getHeight()
	{
		return w_height;
	}

	double getArea()
	{
		return getWidth() * getHeight();
	}


//MODIFIERS
//


//COMPUTATORS

	//based on the cut type, set the dimensions of this wrapper
	void updateDimensions()
	{
		if(base_level)
		{
			//for base level wrappers, the dimensions are equal to the kernel it contains
			w_width = base_kernel->getWidth();
			w_height= base_kernel->getHeight();
		}
		else
		{
			//for non-base level wrappers, dimensions are the combination of w1 and w2,
			//depending on the cut type
			if(cut == 'h')
			{
				w_width = max(w1->getWidth(), w2->getWidth());

				w_height = w1->getHeight() + w2->getHeight();
			}
			else //if not 'h', then assum cut == 'v'
			{
				w_height = max(w1->getHeight(), w2->getHeight());

				w_width = w1->getWidth() + w2->getWidth();
			}

		}
	}
	

	
}; //end Kernel_Wrapper

#endif

