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
#include <vector>
//#include <gdb>

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

	//function to update all performance metrics
	virtual void computePerformance()
	{
		cout << "Kernel.h computePerformance()\n";
		this->computeHeight();
		this->computeWidth();
		this->computeTime();
		this->computeMemory();
	}

	string getType() { return "Kernel"; }

	int getX() { return x; }

	int getY() { return y; }

//MODIFIERS
	void setX(int new_X) { x = new_X; }

	void setY(int new_Y) { y = new_Y; }

	virtual void update() { };

	virtual void setEP(string key, int val)
	{
		cout << "\n****CALLED setEP() from Kernel class****\n";
	}

	//gives an array of ints to help graphically represent the kernel
	//each row in the returned array is one rectangle
	//of the format: [x, y, width, height]
	virtual vector<vector<int>> getRectangles()
	{
		cout << "Get Rects Kernel\n";
		vector<vector<int>> rects;
		return rects;
	}
	
};

int Kernel::kernel_count = 0;

#endif
