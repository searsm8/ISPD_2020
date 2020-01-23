//Kernel.h
//represents a kernel for the Wafer-Scale Engine (WSE)
//kernels can be of many different types, such as:
//conv, dblock, cblock
//each of these types will be defined seperately as an 
//an extension of the Kernel class

#ifndef _Kernel
#define _Kernel

#include <iostream>
#include <map>
#include <string>
#include <cmath>

using namespace std;


class Kernel
{
private:
public:
	int x, y, rotation, ID;
	static int kernel_count;

	map<string, int> FP; //Formal parameters. Immutable.
	map<string, int> EP; //Execution parameters

	int height, width, time, memory;

	//constructors
	Kernel()
	{
		x = 0;
		y = 0;
		rotation = 0;
		ID = kernel_count++;
	}

	void printParameters()
	{
		cout << "\nKernel " << ID << " at (" << x << ", " << y << ", R" << rotation << ")\n";
	
		cout << "Formal Parameters: ";
		map<string, int>::iterator itr;
		for(itr = FP.begin(); itr != FP.end(); ++itr)
			cout << itr->first << "=" << itr->second << " ";
		
		cout << "\nExecution Parameters: ";
		for(itr = EP.begin(); itr != EP.end(); ++itr)
			cout << itr->first << "=" << itr->second <<  " ";
			
		cout << endl << endl;	
	}	

	void printPerformance()
	{
		cout << "Performance metrics for kernel " << ID << ":\n";
		cout << "Height: " << height << endl;
		cout << "Width: "  << width  << endl;
		cout << "Time: "   << time   << endl;
		cout << "Memory: " << memory << endl;
	}

	int computeHeight() { return -1; }

	int computeWidth() { return -1; }

	int computeTime() { return -1; }

	int computeMemory() { return -1; }
	
};
int Kernel::kernel_count = 0;
#endif
