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
	static int kernel_count;

	int x, y, rotation, ID;
	int height, width, time, memory;

	string name;

	map<string, int> FP; //Formal parameters. Should not be changed.
	map<string, int> EP; //Execution parameters

	Kernel* next_kernel; //pointer to the next kernel in the pipeline

	//constructors
	Kernel()
	{
		x = 0;
		y = 0;
		rotation = 0;
		ID = kernel_count++;
		name = "Kernel " + to_string(ID);
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

//ACCESSORS
	string getType() { return "Kernel"; }

	string getName() { return name; }

	virtual string getParamString()
	{
		return "Kernel.h getParamString()";
	}

	int getID() { return ID; }

	int getX() { return x; }

	int getY() { return y; }

	int getWidth() { return width; }

	int getHeight() { return height; }

	int getTime() { return time; }

	int getMemory() { return memory; }

	Kernel* getNextKernel() { return next_kernel; }

	//getRectangles gives an array of ints to help draw the kernel
	//each row in the returned array is one rectangle
	//of the format: [x, y, width, height]
	virtual vector<vector<int>> getRectangles()
	{
		cout << "\n****CALLED getRectangles() from Kernel class****\n";
		vector<vector<int>> rects;
		return rects;
	}

//MODIFIERS
	void setX(int new_X) { x = new_X; }

	void setY(int new_Y) { y = new_Y; }

	void setNextKernel(Kernel* next) { next_kernel = next; }

	void setName(string new_name) { name = new_name; }

	virtual void setEP(string key, int val)
	{
		cout << "\n****CALLED setEP() from Kernel class****\n";
	}

//COMPUTATORS	
	int computeHeight() { return -1; }

	int computeWidth() { return -1; }

	int computeTime() { return -1; }

	int computeMemory() { return -1; }

	//finds the L1 Distance to the next_kernel
	//assumes the width and height are up to date
	int computeL1Distance()
	{
		if(next_kernel == NULL) return 0;

		else return abs((next_kernel->getX()+next_kernel->getWidth()/2)
			- (this->getX()+this->getWidth()/2) )
			+ abs((next_kernel->getY()+next_kernel->getHeight()/2)
			- (this->getY()+this->getHeight()/2) );

	}

	//function to update all performance metrics
	virtual void computePerformance()
	{
		cout << "Kernel.h computePerformance()\n";
		this->computeHeight();
		this->computeWidth();
		this->computeTime();
		this->computeMemory();
	}
};

int Kernel::kernel_count = 0;

#endif
