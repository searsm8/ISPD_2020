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
#define VISUALIZE

#ifdef VISUALIZE
#include "visual/VisualizeWSE.h"
#endif

#include "util.h"
#include "kernel/Kernels.h"
#include <iostream>
#include <fstream>
#include <random>
#include <time.h>


class CometPlacer
{
private:
	int VISUAL_UPDATE_INTERVAL = 10000; //how often the visualization gets updated
	int MAX_ALLOWED_MEMORY = 48000; //for a single core on the WSE
	int wirepenalty; //how much to weight the wirelength
	int timelimit; //runtime limit for program execution
	int width;
	int height;

	//keep pointers to all the kernels in the layout
	vector<Kernel*> kernels;


	Kernel* head; //head of the chain of kernels
	int iteration;
	long avg_time;

	vector<string> slicing; //dictates the kernel layout with a series of slicing steps
	string output;

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
		//create a normal distribution for Aspect Ratios
		random_device rd("default");
		mt19937 gen{rd()};
		
		double mean = 1.5, std_dev = .3;
		normal_distribution<double> d{mean, std_dev};

		for(int i = 0; i < kernels.size(); i++)
		{
			double new_AR = d(gen);

			if(new_AR < 0) new_AR = 1;

			if(rand()%2 == 0)
				new_AR = 1 / new_AR;

		//	new_AR=2;

			kernels[i]->setAR(new_AR);
			cout << kernels[i]->getName() << " AR: " << new_AR << endl;
		}

		setInitialPlacement();
		initializeAnnealing();
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

		int i = 1;
		Kernel* new_kernel;
		if(elements[0].find("dblock") != -1)
		{
			new_kernel = new Dblock(H, W, F, i,i,i,i,i,i,i,i);
		}
		else if(elements[0].find("cblock") != -1)
		{
			new_kernel = new Cblock(H, W, F, i,i,i,i,i,i,i,i,i,i);
		}

		new_kernel->setName(elements[0]);
		kernels.push_back(new_kernel);

	} //end readNode()

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
		k2->setPrevKernel(k1);
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
	
//PRINT METHODS
	
	//print all the kernel's target AR and actual AR
	void printARs()
	{
		cout << endl;

		for(Kernel* k : kernels)
		{
			cout << k->getName() << " targetAR: " << k->getTargetAR() << " actual AR: " << k->getAR() << endl;
		}
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
		cout << "Head Kernel: " << head->getName() << endl;
		cout << "Total Wire Penalty: " << computeL1Penalty() << endl;
		cout << "Max Time: " << getMaxTime() << endl;
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

	void printTimeAndArea()
	{
		for(Kernel* k : kernels)
		{
			cout << k->getName() << " (Time: " << k->getTime() << " (Area: " << k->getArea() << endl;;	
		}
	}


	//visualize the kernels on the WSE
	void updateVisual()
	{
#ifdef VISUALIZE
		for(int i = 0; i < kernels.size(); i++)
		{
			kernels[i]->updateXY();
		}

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

//ACCESSORS
	int getTotalKernelArea()
	{
		int total_area = 0;

		for(Kernel* k : kernels)
			total_area += k->getArea();

		return total_area;
	}

	int getWaferArea() { return width*height; }


	//returns true if all kernels use a legal amount of memory
	//for WSE, each kernel can only use 48kb
	bool checkMemoryLegality()
	{

		return true;
	}


	//returns true if all Kernels placement is legal
	bool checkPlacementLegality()
	{
		bool legalPlacement = true;
		//check for any kernel overlapping any other
		for(int i = 0; i < kernels.size(); i++)
		{
			for(int j = i+1; j < kernels.size(); j++)
			{
				if(kernels[i]->isOverlapping(kernels[j]))
				{
					cout << kernels[i]->getName() << " is overlapping with " << kernels[j]->getName() << endl;
					legalPlacement = false;
				}
			}
		}



		//check for kernels outside boundary
		
		
		if(legalPlacement)
			cout << "No overlapping kernels! Legal Placement!\n";
		else cout << "Overlapping kernels. Illegal Placement.\n";
		return legalPlacement;
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

	long long computeTotalTime()
	{
		long long sum = 0;
		for(Kernel* k : kernels)
			sum += k->getTime();
		
		return sum;
	}

	int computeAvgTime()
	{
		avg_time = computeTotalTime()/kernels.size();
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

	//return pointer to the kernel with the longest execution time
	Kernel* getLongestKernel()
	{
		Kernel* k = kernels[0];
		int longest_time = k->getTime();

		for(int i = 1; i < kernels.size(); i++)
		{
			if(kernels[i]->getTime() > longest_time)
			{
				k = kernels[i];
				longest_time = kernels[i]->getTime();
			}
		}

		return k;
	}

	void increaseAllEP(string key, int increment=1)
	{
		cout << "\nincreaseAllEP("<< key << ", " << increment << ")\n";
		for(int i = 0; i < kernels.size(); i++)
		{
//			cout << "AR of " << kernels[i]->getName() << ": " << kernels[i]->getAR() << endl;
			kernels[i]->increaseEP(key, increment);
		}
	}


	void modifyKernel(int kernel_num, string key, int new_val)
	{
		Kernel* k = kernels[kernel_num];
		cout << endl;
		cout<<"modifyKernel("<<kernel_num<<", "<<key<<", "<<new_val<<")" <<endl;
		double d = k->computeNetBenefitOfIncreasing(key); 
		cout << "Net benefit of increasing " << key << ": " << d << endl;

		k->increaseEPtoNextValue(key);
		k->computePerformance();
		k->printPerformance();
		cout << endl;
	}

	//returns true if any single kernel exceeds the height or width 
	//of the wafer
	bool kernelExceedsWaferSize()
	{
		for(int i = 0; i < kernels.size(); i++)
		{
			if(kernels[i]->getWidth() > width)
			{
				cout << kernels[i]->getName() << " eXceeds wafer width!\n";
				return true;
			}
			if(kernels[i]->getHeight() > height)
			{
				cout << kernels[i]->getName() << " exceeds wafer height!\n";
				return true;
			}
		}

		return false;
	}

	//try to make each kernel have the same time
	//find the kernel with highest time and reduce the time
	//repeat until max time is with allowed_percent_difference 
	//of the avg time
	void equalizeKernelTimes(double allowed_percent_difference = 0.1)
	{
		cout << "\nequalizeKernelTimes()"<<endl;

		//equalize the time within each kernel
		for(int i = 0; i < kernels.size(); i++)
			kernels[i]->equalizeTime();

		//count prevents equalize from increasing forever
		int count = 0, COUNT_MAX = 10*kernels.size();

		while(count < COUNT_MAX)
		{
			Kernel* k = getLongestKernel();	
			if(k->getTime() < (double)avg_time*(1.0+allowed_percent_difference)) 
				break; //equalize condition met
			
			k->increaseSize(); //increases c or k
			//if(k->getEP("h") <= k->getEP("w"))
			//	k->increaseEP("h", 1);
		//	else	k->increaseEP("w", 1);
			computeAvgTime(); //update the avg time
			count++;
		}

		cout << "Equalize count: " << count << endl;

	}

	void maximizeKernelSize(double fill_percent_p1 = 0.75) 
	{
		//try to fill this much percent of the wafer in phase 1
		double filled_percent = (double)(getTotalKernelArea()) / (double)(getWaferArea()); 
		Kernel* k;
		int fail_count = 0;

		//until most of the wafer is filled
		while(filled_percent < fill_percent_p1)
		{
			//slightly increase the size of the largest kernel
			k = getLongestKernel();
			if(k->increaseSize())
			{
				fail_count = 0;
			}
			//if fail to increase size 3 times, max size reached
			else if(++fail_count >=3) 
			{
				break;
			}
			k->computePerformance();
			//printARs();
					

		//	equalizeKernelTimes(0.05);

	cout << "avg_time: " << computeAvgTime() << endl;
	cout << "max_time: " << getMaxTime() << endl;
	printTimeAndArea();
			filled_percent = (double)(getTotalKernelArea()) / (double)(getWaferArea()); 
			cout << "filled_percent: " << filled_percent  << endl << endl;

			if(filled_percent > fill_percent_p1)
				updateVisual();
			
			//if a kernel is too large to fit on wafer, quit
			if(kernelExceedsWaferSize())
			{
				break;
			}
		}
		printARs();
		updateVisual();
	}

	//for each kernel, slightly increase its size until
	//it is very close to targetAR
	void achievePreciseAR(double precision = 0.05)
	{
		Kernel* k;
		for(int i = 0; i < kernels.size(); i++)
		{
			k = kernels[i];
			bool AR_less_than_target = false;
			if(k->getAR() < k->getTargetAR())
				AR_less_than_target = true;

			while(abs((k->getAR() - k->getTargetAR()) / k->getTargetAR()) > precision)
			{
				if(AR_less_than_target) 
				{
					if(k->getAR() > k->getTargetAR())
						break;
				}
				else
				{
					if(k->getAR() < k->getTargetAR())
						break;
				}
				
				if(!(k->increaseSize()))	
					break;

				k->computePerformance();
			}
		}
			double filled_percent = (double)(getTotalKernelArea()) / (double)(getWaferArea()); 
			cout << "filled_percent: " << filled_percent  << endl << endl;

	}

	//check that each kernel meets the memory constraint
	//and does not use more then 48kB per core
	//if a kernel requires too much memory,
	//increase the size
	bool enforceMemoryConstraint()
	{
		Kernel* k;
		for(int i = 0; i < kernels.size(); i++)
		{
			k = kernels[i];
			//memory constraint check here!
			while(k->getMemory() > MAX_ALLOWED_MEMORY)
			{
				k->increaseSize();
				k->computePerformance();
			}

			cout << k->getName() << " meets memory contraints with " << k->getMemory() << " kB/core!\n";
			k->printPerformance();
			updateVisual();
		}

		return true;
	}

	//perform Simulated Annealing on the kernel placement to find a layout
	//Optimize: wirelength
	//Constraints: fits within WSE
	void slicing_layout_SA()
	{

		//the layout is dictated by the order of the kernels
		//and the slicing instructions
		//E.G. k1 k2 h k3 k4 hh
		//
		
		
	}

	//initialize the slicing instructions. 
	void initializeAnnealing()
	{
		//first slicing should always be blank ""
		slicing.push_back("");
		//start out with all horizontal cut 'h' instructions
		//#slices should be 1 less than # kernels
		for(int i = 1; i < kernels.size(); i++)
			slicing.push_back("h");
	}

	bool performAnnealingStep(double temp)
	{
		performMove(temp);
		return evaluateMove(temp);
	}

	void performMove(double temp)
	{
	}

	bool evaluateMove(double temp)
	{
		return true;
	}

	//after finding a layout that fits, give definite (x,y) 
	//coordinates for each kernel
	void solidify_slicing_layout()
	{
	}

	//place the kernels so that they fit in the wafer
	void fitKernelsToWafer()
	{
		Kernel* k = head;
		Kernel* next = k->getNextKernel();
		int row = 0;
		int next_row = k->getBottom();
		bool going_right = true; 
		bool block_placed = false;

		cout << "\n***BEGIN fitKernelsToWafer()***\n";
	
		//traverse the kernels from 1st to last
		while(next != NULL)
		{
			block_placed = false;

			//try to place next on right
			if(going_right)
			if(k->getRight() + next->getWidth() <= width)
			{
				next->setLeft(k->getRight() + 1);
				next->setTop(row);
				block_placed = true;

				if(next->getBottom() > next_row)
					next_row = next->getBottom();
			}

			//try to place next on left
			if(!going_right)
			if(k->getLeft() - next->getWidth() >= 0)
			{
				next->setRight(k->getLeft() - 1);
				next->setTop(row);
				block_placed = true;

				if(next->getBottom() > next_row)
					next_row = next->getBottom();
			}

			//try to place next on bottom
			if(!block_placed)
			if(k->getBottom() + next->getHeight() < height)
			{
				//if on the right half, push to right wall
				if(k->getLeft()+k->getWidth()/2 > width/2)
					next->setRight(width);
				else //else on left half, push to left wall
					next->setLeft(0);

				block_placed = true;
				going_right = !going_right;
				row = next_row;
				next->setY(row);

				next_row = next->getBottom();
			}
			else 
			{
				cout << "NO MORE ROOM FOR KERNELS!!!\n";
			}
			
			updateVisual();
			cout << "Placed kernel: " << next->getName() << endl;
			cout << "(row = " << row << ", next_row = " << next_row << ")\n";

			k = next;
		       	next = next->getNextKernel();
		}
		
		cout << "\n***END fitKernelsToWafer()***\n";
	} //end fitKernelsToWafer()


};

#endif


