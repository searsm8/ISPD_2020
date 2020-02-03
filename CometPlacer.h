//CometPlacer.h
//author: Mark Sears
//
//Defines CometPlacer class, which manipulates kernels 
//in the WSE to optimize performance
#ifndef _CometPlacer
#define _CometPlacer

//if VISUALIZE is defined, then the SDL library
//will be used to create a visualization
//if SDL is not installed, comment this #define
//#define VISUALIZE

#ifdef VISUALIZE
#include "visual/VisualizeWSE.h"
#endif

#include "util.h"
#include "kernel/Kernels.h"
#include <iostream>
#include <fstream>


class CometPlacer
{
private:
	int VISUAL_UPDATE_INTERVAL = 10000;
	int MAX_ALLOWED_MEMORY = 48000;
	string output;
	int wirepenalty;
	int timelimit;
	int width;
	int height;

	vector<Kernel*> kernels;
	Kernel* head; //head of the chain of kernels
	int iteration;
	int avg_time;

#ifdef VISUALIZE
	SDL_Window* window;
#endif

public:


	//constructor
	CometPlacer(string kgraph_filename, string output, int wirepenalty, int timelimit, int width, int height)
	{
		//read the input file and populate the kernels
		readKgraph(kgraph_filename);	
		this->output = output;
		this->wirepenalty = wirepenalty;
		this->timelimit = timelimit;
		this->width = width;
		this->height =height;

		iteration = 0;
		avg_time = 0;
#ifdef VISUALIZE
		window = createWSE(width, height);
#endif
		
		setInitialPlacement();
		computeAvgTime();
	}

	void readNode(string line)
	{
		vector<string> elements = split(line, " ");

		//ignore the input and outputs. No Kernel is created for them
		if(elements[0].find("input") != -1 || elements[0].find("output") != -1)
				return;

		//for other kernels, create the appropriate object

			int F=0, H=0, W=0;
			int R=0, S=0, C=0, K=0, T=0; 

			//read in all the Formal Parameters
			for(int i = 1; i < elements.size(); i++)
			{
				vector<string> next_FP = split(elements[i], "=");

				if(next_FP[0] == "f")
					F = stoi(next_FP[1]);
				else if(next_FP[0] == "h")
					H = stoi(next_FP[1]);
				else if(next_FP[0] == "w")
					W = stoi(next_FP[1]);
			}

		int i = 2;
		if(elements[0].find("dblock") != -1)
		{
			Dblock* new_kernel = new Dblock(H, W, F, i,i,i,i,i,i,i,i);
			new_kernel->setName(elements[0]);
			kernels.push_back(new_kernel);
		}
		else if(elements[0].find("cblock") != -1)
		{
			Cblock* new_kernel = new Cblock(H, W, F, i,i,i,i,i,i,i,i,i,i);
			new_kernel->setName(elements[0]);
			kernels.push_back(new_kernel);
		}
	}

	void readConnection(string line)
	{
		vector<string> elements = split(line, " ");

		string name1 = split(elements[0], ":")[0];
		string name2 = split(elements[2], ":")[0];

		//ignore the shape in elements[3], for now
		//
		//ignore the input and outputs. No Connection is created for them
		if(name2.find("output") != -1)
			return;

		//find the kernels with the names indicated
		Kernel* k1; 
		Kernel* k2; 
		for(int i = 0; i < kernels.size(); i++)
		{
			if(kernels[i]->getName() == name1)
				k1 = kernels[i];
			if(kernels[i]->getName() == name2)
				k2 = kernels[i];
		}

		if(name1.find("input") != -1)
		{
			//set the head to the input	
			head = k2;
			return;
		}

		//set the connection
		k1->setNextKernel(k2);
	}	

	void readKgraph(string kgraph_filename)
	{
		string line;
		ifstream kgraph_file (kgraph_filename);

		if(!kgraph_file.is_open())
		{
			cout << "****WARNING: could not open kgraph input file \""
				<< kgraph_filename << "\"****\n";
			return;
		}

		int mode = 0; //indicates what is being read
			//0: read nodes 
			//1: read connectivity list
		while( getline(kgraph_file, line) )
		{
			//skip commented lines
			if(line[0] == '/' && line[1] == '/') continue;

			if(line.find("Node Definitions") != -1)
			{
				mode = 0;
				continue;
			}
			if(line.find("Connectivity") != -1)
			{
				mode = 1;
				continue;
			}

			switch(mode)
			{
				case 0:
					readNode(line);
					break;
				case 1:
				       	readConnection(line);
					break;
			}
		} //end file input

	}
	
	//print general info about the WSE layout
	void printInfo()
	{
		cout << "\n****WSE INFO****\n";
		cout << "Wafer size: " << width << "x" << height << endl;
		cout << "#Kernels: " << kernels.size() << endl;
		cout << "Iteration: " << iteration << endl; 
		cout << "wirepenalty: " << wirepenalty << endl; 
		cout << "timelimit: " << timelimit << endl; 
		cout << "Total Wire Penalty: " << computeL1Penalty() << endl;
		cout << "Head Kernel: " << head->getName() << endl;
		cout << "avg_time: " << avg_time << endl;
		cout << endl;
	}

	//print the kernel parameters and placements
	void printKernels()
	{
		cout << "\nKernel Information:\n";
		for(Kernel* k : kernels)
		{
			cout << k->getParamString();	
			k->printPerformance();
		}
	}

	//visualize the kernels on the WSE
	void updateVisual()
	{
#ifdef VISUALIZE
		updateWSE(window, kernels, iteration);
		//wait for any key to be pressed
		SDL_Event event;
		while( SDL_WaitEvent(&event) )
		{
			if(event.type == SDL_KEYDOWN)
				break;
				
		}
#endif
		return;
	}


//MODIFIERS

	//compute the L1 Penalty for the whole dam thing
	int computeL1Penalty()
	{
		int dist = 0;

		for(Kernel* k : kernels)
			dist += k->computeL1Distance();

		return dist*wirepenalty;
	}



	//returns true if all kernels use a legal amount of memory
	//for WSE, each kernel can only use 48kb
	bool checkMemoryLegality()
	{

	}


	//returns true if all Kernels placement is legal
	bool checkPlacementLegality()
	{

	}

	//give each kernel an intial placement
	//place all the kernels spread evenly across the wafer in a grid
	void setInitialPlacement()
	{
		int index = 0;

		int grid_size = ceil(sqrt(kernels.size()));
		int spacing = ceil(width/grid_size);
		int next_X, next_Y;

		for(int row = 0; row < grid_size; row++)
		{
			for(int col = 0; col < grid_size; col++)
			{
				if(row%2 == 0)
					next_X = col*spacing;
				else next_X = (grid_size-col-1)*spacing;
				next_Y = row*spacing;

				kernels[index]->setX(next_X);
				kernels[index]->setY(next_Y);
				kernels[index]->computePerformance();
				//kernels[index]->printParameters();
				//kernels[index]->printPerformance();
				index++;	
				if(index >= kernels.size()) break;
			}
			if(index >= kernels.size()) break;
		}

	}

	int computeAvgTime()
	{
		int sum = 0;
		//compute the avg_time
		for(Kernel* k : kernels)
		{
			sum += k->getTime();
		}		
		avg_time = sum/kernels.size();

		return avg_time;
	}

	int getMaxTime()
	{
		int max_time = 0;
		int next_time = 0;
		//compute the avg_time
		for(Kernel* k : kernels)
		{
			next_time = k->getTime();
			if(next_time > max_time)
				max_time = next_time;
		}		

		return max_time;
	}

	//try to make each kernel have the same time
	void equalizeKernelTimes()
	{
		cout << "\nequalizeKernelTimes()"<<endl;
		bool equalized = false;

		while(!equalized)
		{
			bool kernels_modified = false;
			for(int i = 0; i < kernels.size(); i++)
			{
				if(kernels[i]->getTime() > avg_time*1.05)
				{
					kernels[i]->setEP("h",  kernels[i]->EP["h"]+1);
					kernels_modified = true;
					computeAvgTime(); //update the avg time
				}
			}

			if(!kernels_modified)
				equalized = true;
		}
	}

	void increase_EP(string key, int increment=1)
	{
		cout << "\nincrease_EP("<< key << ", " << increment << ")\n";
		for(int i = 0; i < kernels.size(); i++)
		{
			kernels[i]->setEP(key, kernels[i]->EP[key]+increment);
		}
	}


	void modifyKernel(int kernel_num, string key, int new_val)
	{
		Kernel* k = kernels[kernel_num];
		cout << endl;
		cout<<"modifyKernel("<<kernel_num<<", "<<key<<", "<<new_val<<")" <<endl;
		k->setEP(key, new_val);
		k->computePerformance();
		k->printPerformance();
		cout << endl;
	}


};

#endif


