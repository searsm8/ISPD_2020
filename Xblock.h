//Xblock.h
//author: searsm8
//
//extends Kernel
//represents a block which contains internal conv kernels
//such as dblock or cblock
//

#ifndef _Xblock
#define _Xblock

#include "Kernel.h"
#include "Conv.h"
#include <algorithm>

using namespace std;

class Xblock : public Kernel
{
private:
public:
	//each Xblock holds Convolutional Kernels
	vector<Conv> convs;
//	vector<Conv*> convs;
	static int xblock_count;

	Xblock(map<string, int> formal_params, string X_type)
	{
		unsigned int i = 1; //initial EP value
		type = X_type;
		//initialize internal convolutional kernels

		int H = formal_params["H"];
		int W = formal_params["W"];
		int F = formal_params["F"];

		if(type == "dblock")
		{
			convs.reserve(3);
			convs.push_back(Conv(H, W, 1, 1, F,   F/4, 1, i, i, i, i));
			convs.push_back(Conv(H, W, 3, 3, F/4, F/4, 1, i, i, i, i));
			convs.push_back(Conv(H, W, 1, 1, F/4, F,   1, i, i, i, i));
		}

		if(type == "cblock")
		{
			convs.reserve(4);
			convs.push_back(Conv(H, W, 1, 1,  F/2, F/4, 1, i, i, i, i));
			convs.push_back(Conv(H, W, 3, 3,  F/4, F/4, 2, i, i, i, i));
			convs.push_back(Conv(H/2,W/2, 1,1,F/4, F,   1, i, i, i, i));
			convs.push_back(Conv(H, W, 1, 1,  F/2, F,   2, i, i, i, i));
		}
		
		//initialize formal parameters
		FP = formal_params;

		//initialize Execution paramaters
		EP.insert(pair<string, int>("h", i));	
		EP.insert(pair<string, int>("w", i));	
		EP.insert(pair<string, int>("c", i));	
		EP.insert(pair<string, int>("k", i));	
	
		height = -1;
		width = -1;
		time = -1;
		memory = -1;
		
		xblock_count++;
		/*
		name = "k";
	       	if(xblock_count < 10)
		       name += 	"0";
		name += to_string(xblock_count);
		*/
	}
/*	
	//take in a new Execution Parameter
	bool setEP(string key, int val)
	{
		//if single character like "h" or "w"
		if(key.size() == 1)
		{
			switch(key[0])
			{
				case 'h':
					for(unsigned int i = 0; i < convs.size(); i++)
						convs[i].setEP(key, val);
					break;

				case 'w':
					for(unsigned int i = 0; i < convs.size(); i++)
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
	*/

	int computeHeight()
	{
		int max = 0;
		int next;
		for(unsigned int i = 0; i < convs.size(); i++)
		{
			next = convs[i].computeHeight();
			if(next > max)
				max = next;
		}
		height = max;
	//	updateY();
		return max;
	}

	int computeWidth()
	{
		int total_width = 0;
		for(unsigned int i = 0; i < convs.size(); i++)
			total_width += convs[i].computeWidth();
		width = total_width;
	//	updateX();
	       return total_width;	
	}

	int computeTime()
	{
		int max = 0;
		int next;
		for(unsigned int i = 0; i < convs.size(); i++)
		{
			next = convs[i].computeTime();
			if(next > max)
				max = next;
		}
		time = max;
		return max;
	}
	
	int computeMemory()
	{
		int max = 0;
		int next;
		for(unsigned int i = 0; i < convs.size(); i++)
		{
			next = convs[i].computeMemory();
			if(next > max)
				max = next;
		}
		memory = max;
		return max;
	}

	//function to update all performance metrics
	void computePerformance()
	{
		computeHeight();
		computeWidth();
		computeTime();
		computeMemory();
	}

	void setRotation(int new_rot)
	{
//cout << "Xblock setRotation("<< new_rot << ")\n";
		Kernel::setRotation(new_rot);
		for(unsigned int i = 0; i < convs.size(); i++)
			convs[i].setRotation(new_rot);

		updateXY();
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
		if(rotation == 0)
		{
			convs[0].x = new_X;
			for(unsigned int i = 0; i < convs.size()-1; i++)
			{	
				new_X += convs[i].width;
				convs[i+1].x = new_X;
			}
		}
		else
		{
			for(unsigned int i = 0; i < convs.size(); i++)
				convs[i].x = new_X;
		}
	}

	void updateY()
	{
		int new_Y = y;
		if(rotation == 0)
		{
			for(unsigned int i = 0; i < convs.size(); i++)
				convs[i].y = new_Y;
		}
		else
		{
			convs[0].y = new_Y;
			for(unsigned int i = 0; i < convs.size()-1; i++)
			{
				new_Y += convs[i].width;
				convs[i+1].y = new_Y;
			}
		}
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
		for(unsigned int i = 0; i < convs.size(); i++)
			rects.push_back(vector<int>{ convs[i].getX(), convs[i].getY(), convs[i].getWidth(), convs[i].getHeight() });
		return rects;
	}


	//attempt to equalize the computation time for the internal Conv blocks
	void equalizeTime()
	{
		//increase the c2 parameter until conv blocks are equal time
		while(convs[1].time > convs[0].time)
			increaseEP("c2", 1);
	}

//MODIFIERS

	//increase (or decrease) the width of the kernel
	//choose the conv with the longest time
	bool increaseWidth()
	{
		Kernel* longest_conv = getLongestConv();
		longest_conv->EP["k"]++;
		computeWidth();
		return true;
	}

	//increase (or decrease) the width of the kernel
	//choose the conv with the shortest time
	bool decreaseWidth()
	{
		Kernel* shortest_conv = getShortestConv();
		shortest_conv->EP["k"]--;
		computeWidth();
		return true;
	}

//PRINT METHODS


	void printPerformance()
	{
		Kernel::printPerformance();
		for(unsigned int i = 0; i < convs.size(); i++)
			convs[i].printPerformance();
	}
	
	void printParameters()
	{
		Kernel::printParameters();
		vector<int> params = getParameters();

		cout << getName() << " : (";
		for(unsigned int i = 0; i < params.size(); i++)
		{
			cout << params[i] << " ";
		}
		cout << ")" << endl;

		for(unsigned int i = 0; i < convs.size(); i++)
			convs[i].printParameters();

	}

	vector<int> getParameters()
	{
		vector<int> params;
		//add Formal Parameters
		params.push_back(getFP("H"));
		params.push_back(getFP("W"));
		params.push_back(getFP("F"));
		//if(type == "cblock")
		//	params.push_back(getFP("T"));

		//add Execution Parameters
		params.push_back(convs[0].getEP("h"));
		params.push_back(convs[0].getEP("w"));
		
		//extra special parameter???
		//if(type == "cblock")
		//	params.push_back(1);
		//
		for(unsigned int i = 0; i < convs.size(); i++)
			params.push_back(convs[i].getEP("c"));
		for(unsigned int i = 0; i < convs.size(); i++)
			params.push_back(convs[i].getEP("k"));

		return params;
	}

	double computeNetBenefitOfIncreasing(string EP_key)
	{
		double net_benefit = 0;
		for(unsigned int i = 0; i < convs.size(); i++)
		{
			double next = convs[i].computeNetBenefitOfIncreasing(EP_key);
			cout << "Net benefit to increase " << EP_key << " for " << convs[i].getName() << ": " << next << endl;
			if(next > net_benefit)
				net_benefit = next;
		}
		return net_benefit;
	}

       	bool setEPtoNextValue(string EP_key, bool increase=true)
	{
		bool change_made = false;

		double min_EP = convs[0].getNextEPValue(EP_key, increase);
		double next_EP;
		for(unsigned int i = 1; i < convs.size(); i++)
		{
			next_EP = convs[i].getNextEPValue(EP_key, increase);

			if(next_EP < min_EP)
				min_EP = next_EP;
		}
#ifdef DEBUG
	cout << getName() << ": setEPtoNextValue("<<EP_key<<", "<<increase<<") to "<<next_EP<<endl;
#endif

		for(unsigned int i = 0; i < convs.size(); i++)
		{
			if(convs[i].setEP(EP_key, min_EP))
				change_made = true;
		}

		if(change_made)
		{
			EP[EP_key] = min_EP;
			computePerformance();
		}
		else
			cout << "!!!!No changes made for setNextEP(" << EP_key << ") to " << min_EP <<  "\n";
#ifdef DEBUG
		printParameters();
		printPerformance();
#endif

		return change_made;
		
	}
	//
	//returns the Conv that currently has the longest time
	Kernel* getLongestConv()
	{
		Kernel* longest_conv = &convs[0];
		double longest_time = longest_conv->getTime();
		for(unsigned int i = 1; i < convs.size(); i++)
		{
			if(convs[i].getTime() > longest_time)
			{
				longest_conv = &convs[i];
				longest_time = longest_conv->getTime();
			}
		} 
		return longest_conv;	
	}
	
	//returns the Conv that currently has the shortest time
	Kernel* getShortestConv()
	{
		Kernel* shortest_conv = &convs[0];
		double shortest_time = shortest_conv->getTime();
		for(unsigned int i = 1; i < convs.size(); i++)
		{
			if(convs[i].getTime() < shortest_time)
			{
				shortest_conv = &convs[i];
				shortest_time = shortest_conv->getTime();
			}
		} 
		return shortest_conv;	
	}
		
	bool changeWidth(bool increase=true)
	{
	//	cout << "changeWidth() of Xblock: " << getName() << endl;
		Kernel* k;
	        if(increase) k = getLongestConv();
		else k = getShortestConv();

		return k->setEPtoNextValue("k", increase);
	}

	bool changeHeight(bool increase=true)
	{		
	//	cout << "changeHeight() of Xblock: " << getName() << endl;
		string EP_to_increase = "c";

		if(increase)
		{
			//find the smallest EP that affects height
			if(getEP("h") <= getEP("c") && getEP("h") <= getEP("w"))
				EP_to_increase = "h";
			if(getEP("w") <= getEP("c") && getEP("w") <= getEP("h"))
				EP_to_increase = "w";
		}
		else
		{
			//find the largest EP that affects height
			if(getEP("h") >= getEP("c") && getEP("h") >= getEP("w"))
				EP_to_increase = "h";
			if(getEP("w") >= getEP("c") && getEP("w") >= getEP("h"))
				EP_to_increase = "w";
		}
		//increase the next EP	
		bool success = setEPtoNextValue(EP_to_increase, increase);
		return success;
	}

	Kernel* createCopy()
	{
	//	cout << "createCopy() Xblock:\n";
	//	cout << "Original: " << this << endl;

		Xblock* newBlock = new Xblock(*this);
	//	cout << "New Xblock: " << newBlock << endl;
		
		return newBlock;
	}

	void copyDataFrom(Kernel* k)
	{
	//	cout << "Xblock copyDataFrom()\n";
	//	Kernel::copyDataFrom(k);
		this->convs = dynamic_cast<Xblock*>(k)->convs;

	}


};

int Xblock::xblock_count = 0;

#endif
