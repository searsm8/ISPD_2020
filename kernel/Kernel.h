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
#include <ctime>

using namespace std;


class Kernel
{
private:
public:
	static int kernel_count;

	int x, y, rotation, ID;
	int height, width, time, memory;
	double targetAR; //target Aspect Ratio which the kernel will try to achieve
		//Aspect Ratio defined as: height/width

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
		targetAR = 1;
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

	virtual void printPerformance()
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

	virtual string getParamString() { return "Kernel.h getParamString()"; }

	int getID() { return ID; }

	int getX() { return x; }

	int getY() { return y; }

	int getLeft() { return x; }

	int getRight() { return x + width; }

	int getTop() { return y; }

	int getBottom() { return y + height; }

	int getWidth() { return width; }

	int getHeight() { return height; }

	int getArea() { return width*height; }

	int getTime() { return time; }

	int getMemory() { return memory; }

	//return doubles so that division math is correct!
	double getFP(string key) { return FP[key]; }

	double getEP(string key) { return EP[key]; }

	Kernel* getNextKernel() { return next_kernel; }

	double getAR() { return (double)((double)height / (double)width); }

	double getTargetAR() { return targetAR; }

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

	void setLeft(int left) { x = left; } 

	void setRight(int right) { x = right - width; } 

	void setTop(int top) { y = top; } 

	void setBottom(int bottom) { y = bottom - height; } 

	void setNextKernel(Kernel* next) { next_kernel = next; }

	void setName(string new_name) { name = new_name; }

	void setAR(double new_AR) { targetAR = new_AR; }

	virtual void updateXY()
	{
	}

	virtual bool setEP(string key, int val)
	{
		cout << "\n****CALLED setEP() from Kernel class****\n";
		return false;
	}
	
	void increaseEP(string key, int val=1)
	{
		setEP(key, EP[key]+val); 
	}

	virtual bool increaseEPtoNextValue(string EP_key)
	{
		return false;
	}

	//increase the size of the kernel based on the target AR
	//returns true if an increase was made
	virtual bool increaseSize()
	{
		//don't increase an EP beyond the FP!!! it gains no time because ceil()
		if(getAR() < getTargetAR())
		{
			return increaseHeight();
		}
		else
		{
			return increaseWidth();
		}
		
		return false; //no increase was made because already too big!
	}

	virtual bool increaseWidth()
	{
		cout << "CALLED increaseWidth() from Kernel class!" << endl;
		return true;
	}
	
	virtual bool increaseHeight()
	{
		cout << "CALLED increaseHeight() from Kernel class!" << endl;
		return true;
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

	virtual void equalizeTime()
	{
		return;
	}

	//returns true if this kernel is overlapping other Kernel k
	bool isOverlapping(Kernel* k)
	{
		//if one kernel is to the left of the other
		if(x + width < k->x || k->x + k->width < x)
			return false;

		//if one kernel is above the other
		if(y + height < k->y || k->y + k->height < y)
			return false;

		return true;
	}

	//convert and EP string to uppercase, return the FP
	double getAnalogousFP(string EP_key)
	{
		if(EP_key == "h")
			return getFP("H");
		if(EP_key == "w")
			return getFP("W");
		if(EP_key == "c")
			return getFP("C");
		if(EP_key == "k")
			return getFP("K");
	}

	

	virtual double computeNetBenefitOfIncreasing(string EP_key)
	{
		return -1;
	}


};

int Kernel::kernel_count = 0;

#endif
