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
#include <iostream>
#include <fstream>

using namespace std;


class Kernel
{
private:
	bool print = true;
public:
	static int kernel_count;
	string type;

	int x, y, rotation, ID;
	int height, width, time, memory;
	double target_AR; //target Aspect Ratio which the kernel will try to achieve
		//Aspect Ratio defined as: height/width
	double target_time;

	string name;

	map<string, int> FP; //Formal parameters. Should not be changed.
	map<string, int> EP; //Execution parameters

	vector<Kernel*> possible_kernels; //alternative kernels
	int shape_index;

	vector<Kernel*> next_kernels; //pointers to the next kernels in the pipeline
	Kernel* prev_kernel;

	vector<int> colors;

	//constructors
	Kernel()
	{
		x = 0;
		y = 0;
		rotation = 0;
		ID = kernel_count++;
		name = "Kernel " + to_string(ID);
		type = "Kernel";
		//cout << "Kernel constructor: " << name << " : " << type << endl;
		target_AR = 1;

		for(int i = 0; i < 3; i++)
			colors.push_back(rand()%255);
	}

	//destructor
	~Kernel()
	{
		//cout << "Kernel DESTRUCTOR: " << name << " : " << type << endl;
		kernel_count--;
	}

	virtual void printParameters()
	{
		if(!print) return;
		cout << "\nKernel " << ID << " at (" << x << ", " << y << ", R" << rotation << ")\n";
	
		cout << "Formal Parameters: ";
		map<string, int>::iterator itr;
		for(itr = FP.begin(); itr != FP.end(); ++itr)
			cout << itr->first << "=" << itr->second << " ";
		
		cout << "\nExecution Parameters: ";
		for(itr = EP.begin(); itr != EP.end(); ++itr)
			cout << itr->first << "=" << itr->second <<  " ";
		cout << "target_AR: " << target_AR << endl;	
		cout << endl << endl;	
	}	

	virtual vector<int> getParameters()
       	{
		return vector<int>();
	}

	virtual void printPerformance()
	{
		if(!print) return;
		cout << "Performance metrics for kernel " << ID << ":\t";
		cout << "Height: " << height << "\t";
		cout << "Width: "  << width  << "\t";
		cout << "Time: "   << time   << "\t";
		cout << "Memory: " << memory << "\n";
	}

//ACCESSORS
	static int getKernelCount() { return kernel_count; }

	string getType() { return type; }

	string getName() { return name; }

	int getID() { return ID; }

	int getX() { return x; }

	int getY() { return y; }

	int getRotation() { return rotation; }

	int getLeft() { return x; }

	int getRight() { return x + width; }

	int getTop() { return y; }

	int getBottom() { return y + height; }

	int getWidth() { if(rotation == 0) return width; else return height; }

	int getHeight() { if(rotation == 0) return height; else return width; }

	int getArea() { return width*height; }

	int getTime() { return time; }
	
	int getTargetTime() { return target_time; }

	int getMemory() { return memory; }

	vector<int> getColors() { return colors; }

	//return a pair (x, y) specifying the center of this Kernel
	pair<double, double> getCenter()
	{
		pair<double, double > center;

		if(rotation == 0)
		{
			center.first = x + width/2;
			center.second= y + height/2;
		}
		else
		{
			center.first = x + height/2;
			center.second= y + width/2;
		}

		return center;
	}

	//return doubles so that division math is correct!
	double getFP(string key) { return FP[key]; }

	double getEP(string key) { return EP[key]; }

	vector<Kernel*> getNextKernels() { return next_kernels; }

	double getAR() { return (double)((double)height / (double)width); }

	double getTargetAR() { return target_AR; }

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

	virtual void setRotation(int new_rot) { rotation = new_rot; } 
	
	void rotate() { if(rotation == 0) setRotation(90); else setRotation(0); }

	void addNextKernel(Kernel* next) { next_kernels.push_back(next); }

	void setPrevKernel(Kernel* prev) { prev_kernel = prev; }

	void setName(string new_name) { name = new_name; }

	void setAR(double new_AR) { target_AR = new_AR; }

	void setTargetTime(double new_time) { target_time = new_time; }

	//fill in possible_kernels
	void computePossibleKernels()
	{
//cout << "computePossibleKernels()\n";

		vector<double> target_ARs = {0.1, 0.25, 0.33, 0.5, 0.75,  1}; 

		for( int i = 0; i < target_ARs.size(); i++)
		{
			changeShapeToAR(target_ARs[i]);
		
			//if same height as previous, remove previous
			if(possible_kernels.size() > 0 && possible_kernels.back()->getHeight() >= getHeight())
				possible_kernels.pop_back();

			//if different shape found, add current EPs to possible_EPs
			if(possible_kernels.size() == 0 || possible_kernels.back()->getWidth() > getWidth())
				possible_kernels.push_back(createCopy());
		}

		//set to first shape
		shape_index = 0;
		setShape(0);
	}

	vector<Kernel*> getPossibleKernels()
	{
		return possible_kernels;
	}

	void setShape(int index)
	{
//cout << "setShape(" << index << ")" <<endl;
		copyDataFrom(possible_kernels[index]);
		computePerformance();
//cout << "AR: " << getAR() << endl;
//cout << "target_AR: " << getTargetAR() << endl;
//cout << "Time: " << getTime()<< endl;
//cout << "target_time: " << getTargetTime()<< endl;
//cout << "Area: " << getArea()<< endl;
//printParameters();
//printPerformance();
	}

	void nextShape()
	{
//cout << "nextShape()\n";
//cout << "possible_kernels.size(): " << possible_kernels.size() << endl;
		setShape(shape_index);

		shape_index++;
		if(shape_index == possible_kernels.size())
			shape_index = 0;
	}

	//change the shape of the kernel to achieve the new target AR
	void changeShapeToAR(double new_AR)
	{
//cout << "changeShapeToAR()\n";
		target_AR = new_AR;

//cout << "target_time: " << target_time << endl;

		//change the shape repeatedly by increasing and decreasing
		//stop when the target AR is achieved or surpassed.
		bool AR_less_than_target = false;
		if(getAR() < getTargetAR())
			AR_less_than_target = true;

		while(1)
		{
//			cout <<"BEFORE MOVE: Target AR: "<<getTargetAR()<<" Actual AR: " << getAR() << endl;
			//if started less than target AR, but now is greater than target AR, stop
			if(AR_less_than_target) 
			{
				if(getAR() >= getTargetAR())
					break;
			}
			else
			{
				if(getAR() <= getTargetAR())
					break;
			}
					
			if(getTime() > target_time)
			{
				if(!increaseSize())
					break;
			}
			else if(!decreaseSize())
				break;
		
			computePerformance();
//			cout <<"AFTER MOVE: Target AR: "<<getTargetAR()<<" Actual AR: " << getAR() << endl;
		}
		
		computePerformance();
		//try to get back to the original area
		while(getTime() < target_time) 
		{
//cout << "DECREASE SIZE!\n";
//printPerformance();
			if(!decreaseSize())
				break;
			computePerformance();
		}
		while(getTime() > target_time) 
		{
//cout << "INCREASE SIZE!\n";
//printPerformance();
			if(!increaseSize())
				break;
			computePerformance();
		}
//cout << "target_AR: " << target_AR << " actual: " << getAR() << endl;
//printPerformance();

	}

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

	virtual bool setEPtoNextValue(string EP_key, bool increase)
	{
		return false;
	}
	//
	//increase the size of the kernel based on the target AR
	//returns true if an increase was made
	virtual bool increaseSize()
	{
		//don't increase an EP beyond the FP!!! it gains no time because ceil()
		if(getAR() < getTargetAR())
			return changeHeight(true);
		else
			return changeWidth(true);
	}

	//decrease the size of the kernel based on the target AR
	//returns true if an decrease was made
	virtual bool decreaseSize()
	{
		if(getAR() > getTargetAR())
			return changeHeight(false);
		else
			return changeWidth(false);
	}

	virtual bool changeWidth(bool increase) { return true; }
	
	virtual bool changeHeight(bool increase) { return true; }
	
//COMPUTATORS
	int computeHeight() { return -1; }

	int computeWidth() { return -1; }

	int computeTime() { return -1; }

	int computeMemory() { return -1; }

	int computeWirelength() { return computeL1Distance(); }

	//finds the L1 Distance to the next_kernel
	//assumes the width and height are up to date
	double computeL1Distance()
	{
		double dist = 0;
		pair<double, double> c1 = getCenter();
		for(int i = 0; i < next_kernels.size(); i++)
		{
			pair<double, double> c2 = next_kernels[i]->getCenter();
			dist += abs(c1.first - c2.first) + abs(c1.second - c2.second); 

		}
		return dist;

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
		return -1;
	}

	

	virtual double computeNetBenefitOfIncreasing(string EP_key)
	{
		return -1;
	}

	virtual Kernel* createCopy()
	{
//		cout << "createCopy() Kernel:\n";
		return new Kernel(*this);
	}

	//copy ONLY relevant data members into this Kernel
	virtual void copyDataFrom(Kernel* k)
	{
//		cout << "Kernel copyDataFrom()\n";

//		x = k->x;
//		y = k->y;
//		rotation = k->rotation;
		target_AR = k->target_AR;
		FP = k->FP;
		EP = k->EP;
	}


};

int Kernel::kernel_count = 0;

#endif