//CometPlacer.cpp
//author: Mark Sears
//
//Primary driver for Wafer Space Enginer placer
//

#include "CometPlacer.h"

#include <iostream>
#include <fstream>
#include <random>

	//constructor
CometPlacer::CometPlacer(string kgraph_filepath, string output, int wirepenalty, int timelimit, int width, int height)
: output(output), wirepenalty(wirepenalty), timelimit(timelimit), width(width), height(height), iteration(0), avg_time(0), annealer(wirepenalty, width, height)
{
	//read the input file and populate the kernels
	readKgraph(kgraph_filepath);	
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

		new_AR=.25;

		kernels[i]->setAR(new_AR);
#ifdef DEBUG
cout << kernels[i]->getName() << "  AR: " << new_AR << endl;
#endif
	}

	setInitialPlacement();
	computeAvgTime();
}

void CometPlacer::readNode(string line)
{
	vector<string> elements = split(line, " ");
	if(elements.size() == 0) return; //blank line!

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
		new_kernel = new Xblock(H, W, F, "dblock");
		//new_kernel = new Dblock(H, W, F, i,i,i,i,i,i,i,i);
	}
	else if(elements[0].find("cblock") != -1)
	{
		new_kernel = new Xblock(H, W, F, "cblock");
	}

	new_kernel->setName(elements[0]);
	kernels.push_back(new_kernel);

} //end readNode()

void CometPlacer::readConnection(string line)
{
	vector<string> elements = split(line, " ");
	if(elements.size() == 0) return; //blank line!

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
	k1->addNextKernel(k2);
	k2->setPrevKernel(k1);
}	

void CometPlacer::readKgraph(string kgraph_filepath)
{
	string line;
	ifstream kgraph_file (kgraph_filepath);

	if(!kgraph_file.is_open())
	{
		cout << "****WARNING: could not open kgraph input file \""
			<< kgraph_filepath<< "\"****\n";
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
//
//print the current best solution to file
//specifying block placements and parameters
void CometPlacer::printOutputToFile(string output_filepath)
{
	string line;
	ofstream output_file(output_filepath);

	cout << "\n****Printing best layout found to file: " << output_filepath << endl;

	for(int i = 0; i < kernels.size(); i++)
	{
		Kernel* k = kernels[i];
		output_file << "k" << (i+1) << " = " << k->getType() << "( ";
	  	vector<int> params = k->getParameters();     
		for(int i = 0; i < params.size(); i++)
			output_file << params[i] << " ";
		output_file << ")" << endl;
		output_file << "k" << (i+1) << " : place(" << k->getX() << " " << k->getY() << " R" << k->getRotation() << ")" << endl;
	}

}

//print general info about the WSE layout
void CometPlacer::printInfo()
{
	cout << "\n****WSE INFO****\n";
	cout << "Wafer size: " << width << "x" << height << endl;
	cout << "#Kernels: " << kernels.size() << endl;
	cout << "Iteration: " << iteration << endl; 
	cout << "wirepenalty: " << wirepenalty << endl; 
	cout << "timelimit: " << timelimit << endl; 
	cout << "Head Kernel: " << head->getName() << endl;
	cout << "Total Wire Penalty: " << computeL1Penalty() << endl;
	cout << "Max Time: " << getLongestTime() << endl;
	cout << endl;
}

//print the kernel parameters and placements
void CometPlacer::printKernels()
{
	cout << "\nKernel Information:\n";
	for(Kernel* k : kernels)
	{
		k->printPerformance();
	}
}

void CometPlacer::printTimeAndArea()
{
	cout << "Longest Time: " << getLongestTime() << endl;
	for(Kernel* k : kernels)
	{
		cout << k->getName() << " (Time): " << k->getTime() << " (Area): " << k->getArea() << endl;;	
		k->printParameters();
	}
}


//visualize the kernels on the WSE
void CometPlacer::updateVisual(bool wait_for_anykey)
{
#ifdef VISUALIZE
	for(int i = 0; i < kernels.size(); i++)
		kernels[i]->updateXY();

	updateWSE(window, kernels, iteration);
	if(!wait_for_anykey) return;

	//wait for any key to be pressed
	cout << "\nPress 'n' to continue, 'x' to quit...\n";
	SDL_Event event;
	while( SDL_WaitEvent(&event) )
	{
		if(event.type == SDL_KEYDOWN)
		{
			//press "n" for next
			if(event.key.keysym.sym == SDLK_n )
				break;

			//press "x" to kill window and process
			if(event.key.keysym.sym  == SDLK_x )
			{
				cout << "Exiting process from manual command...\n";
				exit(0);
			}
		}
	}
#endif
	return;
}


//MODIFIERS

//compute the L1 Penalty for the whole dam thing
int CometPlacer::computeL1Penalty()
{
	int dist = 0;

	for(Kernel* k : kernels)
		dist += k->computeL1Distance();

	return dist*wirepenalty;
}

//ACCESSORS
int CometPlacer::computeTotalKernelArea()
{
	int total_area = 0;

	for(Kernel* k : kernels)
		total_area += k->getArea();

	return total_area;
}

//give each kernel an intial placement
//place all the kernels spread evenly across the wafer in a grid
void CometPlacer::setInitialPlacement()
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

long long CometPlacer::computeTotalTime()
{
	long long sum = 0;
	for(Kernel* k : kernels)
		sum += k->getTime();
	
	return sum;
}

int CometPlacer::computeAvgTime()
{
	avg_time = computeTotalTime()/kernels.size();
	return avg_time; 
}

int CometPlacer::getLongestTime()
{
	return getLongestKernel()->getTime();
}

//return pointer to the kernel with the longest execution time
Kernel* CometPlacer::getLongestKernel()
{
	Kernel* k = kernels[0];
	int longest_time = k->getTime();

	for(int i = 1; i < kernels.size(); i++)
	{
		if(kernels[i]->getTime() > longest_time)
		{
			k = kernels[i];
			longest_time = k->getTime();
		}
	}

	return k;
}

void CometPlacer::increaseAllEP(string key, int increment)
{
#ifdef DEBUG
cout << "\nincreaseAllEP("<< key << ", " << increment << ")\n";
#endif
	for(int i = 0; i < kernels.size(); i++)
		kernels[i]->increaseEP(key, increment);
}

void CometPlacer::inflateKernelSize(double fill_percent_p1) 
{
	//try to fill this much percent of the wafer in phase 1
	double filled_percent = (double)(computeTotalKernelArea()) / (double)(getWaferArea()); 
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
		filled_percent = (double)(computeTotalKernelArea()) / (double)(getWaferArea()); 

		if(filled_percent > fill_percent_p1)
			updateVisual();
	}

	//After inflating kernels to fill most of the WSE, set
	//the target time for each kernel to be the longest kernel time 
	int longest_time = getLongestTime();

	for(int i = 0; i < kernels.size(); i++)
		kernels[i]->setTargetTime(longest_time);
cout << "filled_percent: " << filled_percent  << endl << endl;
}//end inflateKernelSize()

//check that each kernel meets the memory constraint
//and does not use more then 48kB per core
//if a kernel requires too much memory,
//increase the size
bool CometPlacer::enforceMemoryConstraint()
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

	}

	return true;
}

void CometPlacer::decreaseBlocks()
{
	for(int i = 0; i < kernels.size(); i++)
	{
		Kernel* k = kernels[i];
		k->decreaseSize(); //decrease size
		k->computePerformance();
		updateVisual();
	}
}


//use Slicing_Annealer object to perform Simulated annealing on the kernels
void CometPlacer::performAnnealing()
{
	annealer.setBlocks(kernels);
	annealer.initializeOps();
	annealer.initializeTemp();
	updateVisual(true);

	for(int i = 0; i < 99; i++)
	{
		for(int i = 0; i < 10; i++)
		{
			for(int i = 0; i < 100; i++)
			{
				annealer.performAnnealingStep();
			}
			annealer.reduceTemp();
			kernels = annealer.getBlocks();
			updateVisual(false);
			if(annealer.getTemp() < 1)
				break;
		}
		annealer.WSEcostFunction();
		annealer.printTemp();
		annealer.printCost();
		printTimestamp();
		updateVisual();
		if(annealer.equilibriumReached() || annealer.getTemp() < 1)
		{
			cout << "\n**********\n";
			cout << "EXIT CRITERIA MET..." << endl;
			updateVisual(true);
			annealer.printResults();
			return;
		}
	}
} //performAnnealing

void CometPlacer::computePossibleKernels()
{
	for(int i = 0; i < kernels.size(); i++)
	{
		kernels[i]->computePossibleKernels();
	}
}

//used to track program execution time
std::clock_t start_time = std::clock();
std::clock_t timestamp = std::clock();
std::clock_t time_ref = std::clock();

void CometPlacer::resetTimestamp()
{
	timestamp = clock();
	start_time = clock();
}

//prints how long the program has executed and time since previous timestamp
void CometPlacer::printTimestamp()
{
	double total_time_elapsed  = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
	double time_since_previous = (std::clock() - timestamp) / (double) CLOCKS_PER_SEC;
		
	printf("TIME ELAPSED: %.02f\t(%.02f since previous)\n", total_time_elapsed, time_since_previous);
	
	timestamp = clock();
}

int main(int argc, char** argv)
{
	cout << "****CometPlacer for WSE****\n\n";
	srand(time(0)); //set random seed
	
	string kgraph_filepath; //file to read kgraph input
	string output_filepath; //file to write output to

	//runtime arguments, and defaults
	int wirepenalty = 100;
	int timelimit = 3600;
	int width = 633;
	int height = 633;

	//read the runtime arguments
	for(int i = 1; i < argc; i++)
	{
		vector<string> input = split(argv[i], "=");

		if(input.size() > 1)
		{
			if(input[0] == "kgraph")
			       	kgraph_filepath = input[1];
			if(input[0] == "output")
				output_filepath= input[1];
			if(input[0] == "wirepenalty")
				wirepenalty = stoi(input[1]);
			if(input[0] == "timelimit")
				timelimit= stoi(input[1]);
			if(input[0] == "width")
				width = stoi(input[1]);
			if(input[0] == "height")
				height = stoi(input[1]);
		}
	}

	CometPlacer placer = CometPlacer(kgraph_filepath, output_filepath, wirepenalty, timelimit, width, height);

	placer.updateVisual();
	//placer.printTimeAndArea();
	//placer.printKernels();

	placer.enforceMemoryConstraint();
	placer.updateVisual(true); 
	placer.inflateKernelSize(0.8);
	placer.updateVisual(true); 

	placer.performAnnealing();
	placer.printOutputToFile(output_filepath);

	return 1;
}
