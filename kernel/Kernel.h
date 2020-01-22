//Kernel.h
//represents a kernel for the Wafer-Scale Engine (WSE)
//kernels can be of many different types, such as:
//conv, dblock, cblock
//each of these types will be defined seperately as an 
//an extension of the Kernel class

#ifndef _Kernel
#define _Kernel

#include <iostream>

using namespace std;


class Kernel
{
private:
	int x, y, rotation;
public:
	//constructors
	Kernel()
	{
		x = 0;
		y = 0;
		rotation = 0;
	}

	Kernel(const int& x, const int& y)
	{
		this->x = x;
		this->y = y;
		this->rotation = 0;
	}

	Kernel(const int& x, const int& y, const int& rot)
	{
		this->x = x;
		this->y = y;
		this->rotation = rot;
	}

	void printParams()
	{
		cout << "x: " << x << endl;
		cout << "y: " << y << endl;
		cout << "rot: " << rotation << endl;
	}	

	void printPerformance()
	{
		cout << "Height: " << this->getHeight() << endl;
		cout << "Width: " << getWidth() << endl;
		cout << "Time: " << getTime() << endl;
		cout << "Memory: " << getMemory() << endl;
	}

	int getHeight()
	{
		return -1;
	}

	int getWidth()
	{
		return -1;
	}

	int getTime()
	{
		return -1;
	}

	int getMemory()
	{
		return -1;
	}
};
#endif
