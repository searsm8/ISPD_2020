//Dblock.h
//author: searsm8
//
//extends Kernel
//represents a dblock
//

#ifndef _Dblock
#define _Dblock

#include "Kernel.h"
#include "Conv.h"
#include <algorithm>

using namespace std;

class Dblock : public Kernel
{
private:
public:
	//each Dblock holds 3 Convolutional Kernels
	vector<Conv> convs;
	string EP_to_increase; //denotes the next EP to increase for height

	//constructors
	Dblock()
	{}

	Dblock(int H, int W, int F, int h, int w, int c1, int c2, int c3, int k1, int k2, int k3)
	{
		//initialize 3 internal convolutional kernels
		convs.push_back(Conv(H, W, 1, 1, F, F/4, 1, h, w, c1, k1));
		convs.push_back(Conv(H, W, 3, 3, F/4, F/4, 1, h, w, c2, k2));
		convs.push_back(Conv(H, W, 1, 1, F/4, F, 1, h, w, c3, k3));
		
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
	
		height = -1;
		width = -1;
		time = -1;
		memory = -1;
		EP_to_increase = "c";
	}
	
	//take in a new Execution Parameter
	bool setEP(string key, int val)
	{
		//if single character like "h" or "w"
		if(key.size() == 1)
		{
			switch(key[0])
			{
				case 'h':
					for(int i = 0; i < convs.size(); i++)
						convs[i].setEP(key, val);
					break;

				case 'w':
					for(int i = 0; i < convs.size(); i++)
						convs[i].setEP(key, val);
					break;
				
				case 'c':
					convs[0].setEP("c", val);
					convs[1].setEP("c", floor((9/4)*val));
					convs[2].setEP("c", val);
					break;
	
				case 'k':
					convs[0].setEP("k", val);
					convs[1].setEP("k", val);
					convs[2].setEP("k", val);
					break;
			}
		}
		else //multicharacter specifies a conv such as c1 or k3
		{
			string new_key = key.substr(0, 1);
			int index = stoi(key.substr(1))-1;
			convs[index].setEP(new_key, val);
		}
		EP[key] = val;
		computePerformance();
		return true;
	} 

	int computeHeight()
	{
		height = max({convs[0].computeHeight(), convs[1].computeHeight(), convs[2].computeHeight()});
		updateY();
		return height;
	}

	int computeWidth()
	{
		width = convs[0].computeWidth() + convs[1].computeWidth() + convs[2].computeWidth();
		updateX();
	       return width;	
	}

	int computeTime()
	{
		time = max({convs[0].computeTime(), convs[1].computeTime(), convs[2].computeTime()});
		return time;
	}
	
	int computeMemory()
	{
		memory = max({convs[0].computeMemory(), convs[1].computeMemory(), convs[2].computeMemory()});
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

	string getType() { return "dblock"; }

	string getParamString()
	{
		string params = getName() + ": " + getType() + "( H:" +
		to_string(FP["H"]) + " W:" + to_string(FP["W"]) + " F:" + to_string(FP["F"]) + " "; 
	       	params += "h:" + to_string(convs[0].EP["h"]) + " w:" + to_string(convs[0].EP["w"]) + " ";  
		for(Conv c : convs)
	       		params += "c:"+to_string(c.EP["c"]) + " k:" + to_string(c.EP["k"]) + " "; 
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
	}

	void updateY()
	{
		int new_Y = y;
		convs[0].y = new_Y;
		convs[1].y = new_Y;
		convs[2].y = new_Y;
	}

	void updateXY()
	{
		updateX();
		updateY();
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
		return rects;
	}


	//attempt to equalize the computation time for the internal Conv blocks
	void equalizeTime()
	{
		//increase the c2 parameter until conv blocks are equal time
		while(convs[1].time > convs[0].time)
			increaseEP("c2", 1);
	}

//PRINT METHODS

	void printPerformance()
	{
		Kernel::printPerformance();
		for(int i = 0; i < convs.size(); i++)
			convs[i].printPerformance();
	}

	double computeNetBenefitOfIncreasing(string EP_key)
	{
		double net_benefit = 0;
		for(int i = 0; i < convs.size(); i++)
		{
			double next = convs[i].computeNetBenefitOfIncreasing(EP_key);
			cout << "Net benefit to increase " << EP_key << " for " << convs[i].getName() << ": " << next << endl;
			if(next > net_benefit)
				net_benefit = next;
		}
		return net_benefit;
	}

       	bool increaseEPtoNextValue(string EP_key)
	{
		for(Kernel k : convs)
		{
			k.printParameters();
			k.printPerformance();
		}

		bool change_made = false;

		double min_EP = convs[0].getNextEPValue(EP_key);
		double next_EP = min_EP;
		for(int i = 1; i < convs.size(); i++)
		{
			next_EP = convs[i].getNextEPValue(EP_key);

			if(next_EP < min_EP)
				min_EP = next_EP;
		}

	cout << getName() << ": increaseEPtoNextValue("<<EP_key<<") to "<<next_EP<<endl;

		for(int i = 0; i < convs.size(); i++)
		{
			if(convs[i].setEP(EP_key, min_EP))
				change_made = true;
		}

		if(!change_made)
		cout << "!!!!No changes made for setNextEP(" << EP_key << ") to " << min_EP <<  "\n";

		return change_made;
		
	}

	//returns the Conv that currently has the longest time
	Kernel* getLongestConv()
	{
		Kernel* longest_conv = &convs[0];
		double longest_time = longest_conv->getTime();
		for(int i = 1; i < convs.size(); i++)
		{
			if(convs[i].getTime() > longest_time)
			{
				longest_conv = &convs[i];
				longest_time = longest_conv->getTime();
			}
		} 
		return longest_conv;	
	}
		
	bool increaseWidth()
	{
		cout << "increaseWidth() of Dblock: " << getName() << endl;
		Kernel* k = getLongestConv();
		if(k->increaseEPtoNextValue("k"))
			return true;
		//if unable to increase width anymore, must increase height instead
		//else return increaseHeight();
		else return false;
	}

	bool increaseHeight()
	{		
		cout << "increaseHeight() of Dblock: " << getName() << endl;
		cout << "EP_to_increase: " << EP_to_increase << endl;
		Kernel* k = getLongestConv();

		//increase the next EP	
		bool success = increaseEPtoNextValue(EP_to_increase);

		//change the EP to be increased
		if(EP_to_increase == "c")
		{
			EP_to_increase = "h";
		}
		else if(EP_to_increase == "h")
		{
			EP_to_increase = "w";
		}
		else if(EP_to_increase == "w")
		{
			EP_to_increase = "c";
		}

		return success;
	}


};
#endif
