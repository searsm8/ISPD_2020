//Conv.h
//represents a Convolutional kernel

#ifndef _Conv
#define _Conv

#include "Kernel.h"


class Conv : public Kernel
{
public:
	//constructors
	//
	Conv()
	{}

	Conv(int H, int W, int R, int S, int C, int K, int T, int h, int w, int c, int k)
	{
		FP["H"] = H;
		FP["W"] = W;
		FP["R"] = R;
		FP["S"] = S;
		FP["C"] = C;
		FP["K"] = K;
		FP["T"] = T;

		EP["h"] = h;	
		EP["w"] = w;	
		EP["c"] = c;	
		EP["k"] = k;	

		type = "conv";

	}


	Conv(map<string, int> formal_params, int WSE_width, int WSE_height)
	{
		MAX_WIDTH = WSE_width;
		MAX_HEIGHT = WSE_height;
		unsigned int i = 1; //initial EP value
		
		//initialize formal parameters
		FP = formal_params;

		//initialize Execution paramaters
		EP.insert(pair<string, int>("h", i));	
		EP.insert(pair<string, int>("w", i));	
		EP.insert(pair<string, int>("c", i));	
		EP.insert(pair<string, int>("k", i));	
	
		type = "conv";
	}

	int computeHeight()
	{
		height = EP["h"] * EP["w"] * (EP["c"]+1);
		return height;
	}

	int computeWidth()
	{
		width = 3 * EP["k"];
		return width;
	}

	int computeTime()
	{
		time = ceil(getFP("H")/getEP("h")) * ceil(getFP("W")/getEP("w")) * ceil(getFP("C")/getEP("c")) * ceil(getFP("K")/getEP("k")) * (getFP("R") * getFP("S") / getFP("T") / getFP("T"));

		return time;
	}

	int computeMemory()
	{
		memory = (getFP("C")/getEP("c")) * (getFP("K")/getEP("k")) * getFP("R") * getFP("S") +
			((getFP("W")+getFP("S")-1)/getEP("w")) * ((getFP("H")+getFP("R")-1)/getEP("h")) * (getFP("K")/getEP("k"));
	//	memory = ceil(getFP("C")/getEP("c")) * ceil(getFP("K")/getEP("k")) * getFP("R") * getFP("S") +
	//		ceil((getFP("W")+getFP("S")-1)/getEP("w")) * ceil((getFP("H")+getFP("R")-1)/getEP("h")) * ceil(getFP("K")/getEP("k"));
		return memory;
	}

	string getType() { return "conv"; }

	string getParamString()
	{
		return  getName() + ": " + getType() + "( H:" +
		to_string(FP["H"]) + " W:" + to_string(FP["W"]) + " F:" +
		to_string(FP["F"]) + " R:" + to_string(FP["R"]) + " S:" +
		to_string(FP["S"]) + " C:" + to_string(FP["C"]) + " K:" +
		to_string(FP["K"]) + " K:" + to_string(FP["T"]) + " h:" +
		to_string(EP["h"]) + " w:" + to_string(EP["w"]) + " c:" +
		to_string(EP["c"]) + " k:" + to_string(EP["k"]) + " )\n";
	}
	
	//function to update all performance metrics
	void computePerformance()
	{
		computeHeight();
		computeWidth();
		computeTime();
		computeMemory();
	}

//MODIFIERS
	void setX(int new_X) { x = new_X; }
	
	void setY(int new_Y) { y = new_Y; }

	bool setEP(string key, int val)
	{
		if(EP[key] == val)
			return false;

		EP[key] = val;
	//	computePerformance(); //is this needed here?
		return true;
	}

	//gives an array of ints to help graphically represent the kernel
	//each row in the returned array is one rectangle
	//of the format: [x, y, width, height]
	vector<vector<int>> getRectangles()
	{
		vector<vector<int>> rects;
		rects.push_back(vector<int>{ x, y, width, height});
		return rects;
	}

	//compute a net benefit index which for increasing an Execution Parameter
	//to the next impactful value
	//(not all EP increases are impactful due to the ceil() function)
	//a "net benefit" of 1 means that increasing the height will yield
	//a proportional decrease in time for this kernel
	double computeNetBenefitOfIncreasing(string EP_key)
	{
		double my_FP = getAnalogousFP(EP_key);
		
		if(my_FP == 0)
			return 0;

		double orig_EP = getEP(EP_key);
		if(orig_EP >= my_FP)
			return 0;
		
		double orig_time_term = ceil(my_FP / orig_EP);

		double new_EP = ceil(my_FP / (orig_time_term - 1));

		double new_time_term = ceil(my_FP / new_EP);
		
		//account for (c+1) in height
		if(EP_key == "c")
		{
			orig_EP++;
			new_EP++;
		}

		double net_benefit = (orig_time_term/new_time_term) / (new_EP/orig_EP) ;
		
		return net_benefit;
		
	}

	//find the next EP value that actually benefits the time performance
	//of this kernel. 
	//Not all EP value increases will benefit the time due to ceil()
	bool setEPtoNextValue(string EP_key, bool increase=true)
	{
		return setEP(EP_key, getNextEPValue(EP_key, increase));
	}


	//return the next EP value for an increase or decrease
	double getNextEPValue(string EP_key, bool increase=true)
	{
		double my_FP = getAnalogousFP(EP_key);
		
		double orig_EP = getEP(EP_key);
		
		if(my_FP == 0)
		{
			cout << getName() << ": " << "INVALID EP_key in Conv.h getNextEPValue(): " << EP_key << endl;
			return orig_EP;
		}

		if(increase && orig_EP >= my_FP)
		{
			cout << getName() << ": " << "No increase possible for EP: " << EP_key << endl;
			return orig_EP;
		}
		if(!increase && orig_EP <= 1)
		{
			cout << getName() << ": " << "No decrease possible for EP: " << EP_key << endl;
			return orig_EP;
		}
		
		double orig_time_term = ceil(my_FP / orig_EP);

		double new_EP;
		if(increase)
			new_EP = ceil(my_FP / (orig_time_term - 1));
		else new_EP = floor(my_FP / (orig_time_term + 1));
		if(new_EP == orig_EP)
			new_EP += (increase ? 1 : -1);

//		cout << "getNextEPValue("<<EP_key<<", "<<increase<<") from " << orig_EP << " to " << new_EP << endl;

		//don't increase beyond FP!
		if(increase && new_EP > my_FP) 
			return orig_EP;

		return new_EP;
	}

	bool changeWidth(bool increase=true)
	{
		return setEPtoNextValue("k", increase);
	}

	bool changeHeight(bool increase=true)
	{
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
		return setEPtoNextValue(EP_to_increase, increase);
	}
	

};
#endif

