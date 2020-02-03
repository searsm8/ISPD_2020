//Cblock.h
//author: searsm8
//
//extends Kernel
//represents a cblock
//

#ifndef _Cblock
#define _Cblock

#include "Kernel.h"
#include "Conv.h"
#include <algorithm>

using namespace std;

class Cblock : public Kernel
{
private:
public:
	//each Cblock holds 4 Convolutional Kernels
	vector<Conv> convs;

	//constructors
	Cblock()
	{}

	Cblock(int H, int W, int F, int h, int w, int c1, int c2, int c3, int c4, int k1, int k2, int k3, int k4)
	{
		//initialize 3 internal convolutional kernels
		convs.push_back(Conv(H, W, 1, 1, F, F/4, 1, h, w, c1, k1));
		convs.push_back(Conv(H, W, 3, 3, F/4, F/4, 2, h, w, c2, k2));
		convs.push_back(Conv(H, W, 1, 1, F/4, F, 1, h, w, c3, k3));
		convs.push_back(Conv(H, W, 1, 1, F/2, F, 2, h, w, c4, k4));
		
		//initialize formal parameters
		FP.insert(pair<string, int>("H", H));	
		FP.insert(pair<string, int>("W", W));	
		FP.insert(pair<string, int>("F", F));	

		//initialize Execution paramaters
		EP.insert(pair<string, int>("h", h));	
		EP.insert(pair<string, int>("w", w));	
		EP.insert(pair<string, int>("c", c1));	
		EP.insert(pair<string, int>("k", k1));	
		EP.insert(pair<string, int>("c1", c1));	
		EP.insert(pair<string, int>("k1", k1));	
		EP.insert(pair<string, int>("c2", c2));	
		EP.insert(pair<string, int>("k2", k2));	
		EP.insert(pair<string, int>("c3", c3));	
		EP.insert(pair<string, int>("k3", k3));	
		EP.insert(pair<string, int>("c4", c3));	
		EP.insert(pair<string, int>("k4", k4));	

		height = -1;
		width = -1;
		time = -1;
		memory = -1;
		
	}

	//take in a new Execution Parameter
	void setEP(string key, int val)
	{
		//if single character like "h" or "w"
		if(key.size() == 1)
			for(int i = 0; i < convs.size(); i++)
				convs[i].setEP(key, val);
		else //multicharacter specifies a conv such as c1 or k3
		{
			string new_key = key.substr(0, 1);
			int index = stoi(key.substr(1))-1;
			convs[index].setEP(new_key, val);
		}
		EP[key] = val;
		computePerformance();
	}

	int computeHeight()
	{
		height = max({convs[0].computeHeight(), convs[1].computeHeight(), convs[2].computeHeight(), convs[3].computeHeight()});
		updateY();
		return height;
	}

	int computeWidth()
	{
		width = convs[0].computeWidth() + convs[1].computeWidth() + convs[2].computeWidth() + convs[3].computeWidth();;
		updateX();
	       return width;	
	}

	int computeTime()
	{
		time = max({convs[0].computeTime(), convs[1].computeTime(), convs[2].computeTime(), convs[3].computeTime()});
		return time;
	}
	
	int computeMemory()
	{
		memory = max({convs[0].computeMemory(), convs[1].computeMemory(), convs[2].computeMemory(), convs[3].computeMemory()});
		return memory;
	}

	//function to update all performance metrics
	void computePerformance()
	{
		computeHeight();
		computeWidth();
		computeTime();
		computeMemory();
	}

	string getType() { return "cblock"; }
	
	string getParamString()
	{
		string params = getName() + ": " + getType() + "( " +
		to_string(FP["H"]) + " " + to_string(FP["W"]) + " " + to_string(FP["F"]) + " "; 
	       	params += to_string(convs[0].EP["h"]) + " " + to_string(convs[0].EP["w"]) + " ";  
		for(Conv c : convs)
	       		params += to_string(c.EP["c"]) + " " + to_string(c.EP["k"]) + " "; 
		params += ")\n";

		return params;
	}


	//set the X coordinate for this Kernel
	//update the X for all internel Conv blocks
	void setX(int new_X)
	{
		x = new_X;
		updateX();
	}

	void setY(int new_Y)
	{
		y = new_Y;
		updateY();
	}

	void updateX()
	{
		int new_X = x;
		convs[0].x = new_X;
		new_X += convs[0].width;
		convs[1].x = new_X;
		new_X += convs[1].width;
		convs[2].x = new_X;
		new_X += convs[2].width;
		convs[3].x = new_X;
	}

	void updateY()
	{
		int new_Y = y;
		convs[0].y = new_Y;
		convs[1].y = new_Y;
		convs[2].y = new_Y;
		convs[3].y = new_Y;
	}

	//gives an array of ints to help graphically represent the kernel
	//each row in the returned array is one rectangle
	//of the format: [x, y, width, height]
	vector<vector<int>> getRectangles()
	{
		vector<vector<int>> rects;
		rects.push_back(vector<int>{ convs[0].x, convs[0].y, convs[0].width, convs[0].height});
		rects.push_back(vector<int>{ convs[1].x, convs[1].y, convs[1].width, convs[1].height});
		rects.push_back(vector<int>{ convs[2].x, convs[2].y, convs[2].width, convs[2].height});
		rects.push_back(vector<int>{ convs[3].x, convs[3].y, convs[3].width, convs[3].height});
		return rects;
	}
};
#endif
