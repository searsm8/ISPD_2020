//CometPlacer.cpp
//author: Mark Sears
//Primary driver for Wafer Space Enginer placer
//
//if VISUALIZE is defined, then the SDL library will be used to create a visualization
//if SDL is not installed, comment this #define
//
#define VISUALIZE

//DEBUG controls print statements
//
//#define DEBUG

#ifdef VISUALIZE
#include "VisualizeWSE.h"
#endif

#include "CometPlacer.h"
#include <chrono>

//GLOBAL VARIABLES
//
//used to track program execution time
//std::clock_t start_time = std::clock();
//std::clock_t timestamp = std::clock();
//std::clock_t time_ref = std::clock();
unsigned long start_time = std::clock();
unsigned long timestamp = std::clock();
unsigned long time_ref = std::clock();
auto start = chrono::steady_clock::now();

	//CONSTRUCTORS
CometPlacer::CometPlacer()
{}

CometPlacer::CometPlacer(string kgraph_filepath, string output, int wirepenalty, int timelimit, int width, int height)
: wirepenalty(wirepenalty), timelimit(timelimit), width(width), height(height), iteration(0), avg_time(0), output(output), annealer(wirepenalty, width, height)
{

	//read the input file and populate the kernels
	readKgraph(kgraph_filepath);	
	WSE_width = width;
	WSE_height = height;
	
#ifdef VISUALIZE
	window = createWSE(width, height);
#endif
	//create a normal distribution for Aspect Ratios
	random_device rd("default");
	mt19937 gen{rd()};
	
	double mean = 1.5, std_dev = .3;
	normal_distribution<double> d{mean, std_dev};

	for(unsigned int i = 0; i < kernels.size(); i++)
	{
		double new_AR = d(gen);

		if(new_AR < 0) new_AR = 1;

		if(rand()%2 == 0)
			new_AR = 1 / new_AR;

		new_AR=.25; //force initial ARs

		kernels[i]->setAR(new_AR);
#ifdef DEBUG
cout << kernels[i]->getName() << "  AR: " << new_AR << endl;
#endif
	}

	assert(kernels.size() != 0);
	setInitialPlacement();
	computeAvgTime();

	for(unsigned int i = 0; i < kernels.size(); i++)
		fixName(kernels[i], i+1);
}


void CometPlacer::readParameter(string line)
{
#ifdef DEBUG
cout << "Begin readParameter()" << endl;;
#endif
	vector<string> elements = split(line, "=");
	if(elements.size() == 0) return; //blank line!

	if((signed int)elements[0].find("width") != -1)
	{
		width = stoi(elements[1]);
		WSE_width = width;
	}
	else if((signed int)elements[0].find("height") != -1)
	{
		height = stoi(elements[1]);
		WSE_height = height;
	}
	else if((signed int)elements[0].find("wlength") != -1)
		wirepenalty = stoi(elements[1]);
	else if((signed int)elements[0].find("memlimit") != -1)
		MAX_ALLOWED_MEMORY = stoi(elements[1]);

}

void CometPlacer::readNode(string line)
{
#ifdef DEBUG
cout << "Begin readNode()" << endl;;
#endif
	//cout << "readNode()\nline: " << line << endl;
	vector<string> elements = split(line, " ");
	if(elements.size() == 0) return; //blank line!

	//handle the case of "dblock[ 1]"
	for(unsigned int i = 0; i < elements.size()-1; i++)
	{
		if(elements[i].back() == '[')
		{
			elements[i] += elements[i+1];
			elements.erase(elements.begin()+i+1);
		}
	}

	//ignore the input and outputs. No Kernel is created for them
	if((signed int)elements[0].find("input") != -1 || (signed int)elements[0].find("output") != -1)
		return;

	//for other kernels, create the appropriate object
		map<string, int> formal_params;
		string kernel_name;
		//read in all the Formal Parameters
		for(unsigned int i = 1; i < elements.size(); i++)
		{
			vector<string> next_FP = split(elements[i], "=");

			if(next_FP[0] == "name")
			{
				kernel_name = next_FP[1];
				continue;
			}

			next_FP[0] = toupper((char &)next_FP[0][0]);

			formal_params[next_FP[0]] = stoi(next_FP[1]);
		}

	//unsigned int i = 1;
	Kernel* new_kernel = NULL;
	if((signed int)elements[0].find("dblock") != -1)
	{
		new_kernel = new Xblock(formal_params, "dblock", width, height);
	}
	else if((signed int)elements[0].find("cblock") != -1)
	{
		new_kernel = new Xblock(formal_params, "cblock", width, height);
	}
	else if((signed int)elements[0].find("conv") != -1)
	{
		//new_kernel = new Conv(formal_params, width, height);
		new_kernel = new Xblock(formal_params, "conv", width, height);
	}

	assert(new_kernel != NULL);
	
	new_kernel->setName(elements[0]);
	kernels.push_back(new_kernel);

} //end readNode()

void CometPlacer::readConnection(string line)
{
#ifdef DEBUG
cout << "Begin readConnection()" << endl;;
#endif
	string delimiter = " ";
	if(line.find("\t") != string::npos)
		delimiter = "\t";
	vector<string> elements = split(line, delimiter);
	if(elements.size() == 0) return; //blank line!

	//handle the case of "dblock[ 1]"
	for(unsigned int i = 0; i < elements.size()-1; i++)
	{
		if(elements[i].back() == '[')
		{
			elements[i] += elements[i+1];
			elements.erase(elements.begin()+i+1);
		}
	}

#ifdef DEBUG
cout << "elements[0]: " << elements[0]<< endl;
cout << "elements[2]: " << elements[2]<< endl;
#endif
	string name1 = split(elements[0], ":")[0];
	string name2 = split(elements[2], ":")[0];
#ifdef DEBUG
cout << "name1: " << name1 << endl;;
cout << "name2: " << name2 << endl;;
#endif

	//ignore the shape in elements[3], for now
	//
	//ignore the input and outputs. No Connection is created for them
	if((signed int)name2.find("output") != -1)
		return;

	//find the kernels with the names indicated
	Kernel* k1 = NULL; 
	Kernel* k2 = NULL; 
	for(unsigned int i = 0; i < kernels.size(); i++)
	{
		if(kernels[i]->getName() == name1)
			k1 = kernels[i];
		if(kernels[i]->getName() == name2)
			k2 = kernels[i];
	}
	if(k2 == NULL)
		return;

	if((signed int)name1.find("input") != -1)
	{
		//set the head to the input	
		head = k2;
		return;
	}

	if(k1 == NULL)
		return;

	//set the connection
	k1->addNextKernel(k2);
	k2->setPrevKernel(k1);
}	

void CometPlacer::readKgraph(string kgraph_filepath)
{
#ifdef DEBUG
cout << "Begin readKgraph()" <<endl;
#endif
	char c_line[1000];
	string line, prev_line;
	int mode = 1; //indicates what is being read
	while( true )
	{
		//if( feof(kgraph_file)) cout << "feof!" << " size: " << kernels.size() << "\n";
		scanf("%[^\n]\n", c_line);

		line = string(c_line);
		if(line == prev_line)
			break;
		prev_line = line;
		//line = "cblock[1] f=128 h=92 w=92";
		//skip commented lines
		if(line[0] == '/' && line[1] == '/') continue;

		if((signed int)line.find("Solver Parameters") != -1)
		{
			mode = 1;
			continue;
		}
		if((signed int)line.find("Node Definitions") != -1)
		{
			mode = 2;
			continue;
		}
		if((signed int)line.find("Connectivity") != -1)
		{
			mode = 3;
			continue;
		}

		switch(mode)
		{
			case 0:
				break;
			case 1:
				readParameter(line);
				break;
			case 2:
				readNode(line);
				break;
			case 3:
				readConnection(line);
				break;
		}
	} //end file input
#ifdef DEBUG
cout << "End readKgraph()" <<endl;
#endif
}

//PRINT METHODS
//

void CometPlacer::printPercentFilled()
{
	double filled_percent = (double)(computeTotalKernelArea()) / (double)(getWaferArea()); 
	cout << "Kernels fill " << 100*filled_percent << "% of wafer." << endl;
}

//fix Kernel names so they match expected output format e.g. "k03"
void CometPlacer::fixName(Kernel* k, int ID)
{
	string new_name = "k";
	if(ID < 10)
		new_name += "0";
	new_name += to_string(ID);

	k->setName(new_name);
}

vector<Kernel*> printed_kernels;
//print the current best solution to file
//specifying block placements and parameters
void CometPlacer::printOutputToFile(string output_filepath)
{
	string line;
	ofstream output_file(output_filepath);

	cout << "\n****Printing best layout found to file: " << output_filepath << endl;

	vector<Kernel*> best_kernels = annealer.getBestBlocks();

	output_file << "include \"common.paint\"" << endl;
	cout << "include \"common.paint\"" << endl;

	//print the kernels in the order from input to ouput
	//start with HEAD, call recursively
	printed_kernels.clear();
	printKernelToFile(head, output_file);

	//print last line
	output_file << "kmap = union( ";	
	cout << "kmap = union( ";	
	printed_kernels.clear();
	printUnion(head, output_file);
	output_file << ")\n";
	cout << ")\n";
}

void CometPlacer::printUnion(Kernel* k, ofstream& output_file)
{
	output_file << k->getName() << " ";
	cout << k->getName() << " ";

	//call recursively to all kernels that this kernel points to
	vector<Kernel*> next_kernels = k->getNextKernels();
	printed_kernels.push_back(k);

	for(unsigned int i = 0; i < next_kernels.size(); i++)
		if(find(printed_kernels.begin(), printed_kernels.end(), next_kernels[i]) == printed_kernels.end())
		printUnion(next_kernels[i], output_file);
}

void CometPlacer::printKernelToFile(Kernel* k, ofstream& output_file)
{
	output_file << k->getName() << " = " << k->getType() << "( ";
	cout << k->getName() << " = " << k->getType() << "( ";
	vector<int> params = k->getParameters();     
	for(unsigned int i = 0; i < params.size(); i++)
	{
		output_file << params[i] << " ";
		cout << params[i] << " ";
	}
	output_file << ")" << endl;
	cout << ")" << endl;


	output_file << k->getName() << " : name('" << k->getName() << ")" << endl;;
	cout << k->getName() << " : name('" << k->getName() << ")" << endl;;

	output_file << k->getName() << " : place(" << k->getX() << " " << k->getY() << " R" << k->getRotation() << ")" << endl;
	cout << k->getName() << " : place(" << k->getX() << " " << k->getY() << " R" << k->getRotation() << ")" << endl;

	printed_kernels.push_back(k);

	//call recursively to all kernels that this kernel points to
	vector<Kernel*> next_kernels = k->getNextKernels();

	for(unsigned int i = 0; i < next_kernels.size(); i++)
		if(find(printed_kernels.begin(), printed_kernels.end(), next_kernels[i]) == printed_kernels.end())
		printKernelToFile(next_kernels[i], output_file);
} //end printKernelToFile()

//print general info about the WSE layout
void CometPlacer::printInfo()
{
	cout << "\n****WSE INFO****\n";
	cout << "Max Wafer size: " << width << "x" << height << endl;
	cout << "Used space: " << annealer.getLayoutWidth() << "x" << annealer.getLayoutHeight() << " = " << annealer.getLayoutWidth() * annealer.getLayoutHeight() << endl;
	cout << "# of Kernels: " << kernels.size() << endl;
	cout << "wirepenalty: " << wirepenalty << endl; 
	cout << "timelimit: " << timelimit << "s " << endl; 
	cout << "memlimit: " << MAX_ALLOWED_MEMORY << endl; 
	cout << endl;
}

//print the kernel parameters and placements
void CometPlacer::printKernels()
{
	cout << "\nKernel Information:\n";
	cout << "Longest Time: " << getLongestTime() << endl;
	cout << "Shortest Time: " << getShortestTime() << endl;
	cout << "Average Time: " << getAverageTime() << endl;
	cout << "Target Time: " << head->getTargetTime() << endl;

	return;

	for(Kernel* k : kernels)
	{
		k->printPerformance();
	}
}

void CometPlacer::printTimeAndArea()
{
	for(Kernel* k : kernels)
	{
		cout << k->getName() << " (Time): " << k->getTime() << " (Area): " << k->getArea() << endl;;	
		k->printParameters();
	}
}


enum PAUSE
{
	PAUSE = true,
	NOPAUSE = false
};

//visualize the kernels on the WSE
void CometPlacer::updateVisual(bool wait_for_anykey, int epoch_count)
{
#ifdef VISUALIZE
	for(unsigned int i = 0; i < kernels.size(); i++)
		kernels[i]->updateXY();

	updateWSE(window, kernels, epoch_count);
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
	unsigned int index = 0;

	int grid_size = ceil(sqrt(kernels.size()));
	assert(grid_size > 0);
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

int CometPlacer::getAverageTime()
{
	int sum_time = 0;
	for(unsigned int i = 0; i < kernels.size(); i++)
		sum_time += kernels[i]->getTime();

	return sum_time/kernels.size();
}

int CometPlacer::getShortestTime()
{
	return getShortestKernel()->getTime();
}

//return pointer to the kernel with the longest execution time
Kernel* CometPlacer::getShortestKernel()
{
	Kernel* k = kernels[0];
	int shortest_time = k->getTime();

	for(unsigned int i = 1; i < kernels.size(); i++)
	{
		if(kernels[i]->getTime() < shortest_time)
		{
			k = kernels[i];
			shortest_time = k->getTime();
		}
	}

	return k;
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

	for(unsigned int i = 1; i < kernels.size(); i++)
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
	for(unsigned int i = 0; i < kernels.size(); i++)
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
	//the target time for each kernel to be the avg kernel time 
	int avg_time = getAverageTime();

	for(unsigned int i = 0; i < kernels.size(); i++)
		kernels[i]->setTargetTime(avg_time);
cout << "filled_percent: " << filled_percent  << endl << endl;
}//end inflateKernelSize()

//check that each kernel meets the memory constraint
//and does not use more then 48kB per core
//if a kernel requires too much memory,
//increase the size
bool CometPlacer::enforceMemoryConstraint()
{
	Kernel* k;
	for(unsigned int i = 0; i < kernels.size(); i++)
	{
		k = kernels[i];
		//memory constraint check here!
		while(k->getMemory() > MAX_ALLOWED_MEMORY)
		{
//			cout << "To meet memory constraint, increased size of " << k->getName() << "\t(Memory = " << k->getMemory() << ")" << endl;
			k->increaseSize();
			k->computePerformance();
		}

	//	cout << k->getName() << " meets memory constraints with " << k->getMemory() << " kB/core!\n";

	}

	return true;
}

void CometPlacer::decreaseBlocks()
{
	for(unsigned int i = 0; i < kernels.size(); i++)
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
	//annealer.setBlocks(kernels);
	annealer.setHead(head);
	annealer.initializeOps();
	annealer.initializeTemp();
	updateVisual(NOPAUSE);

	while(!annealer.equilibriumReached()) //perform annealing steps until exit criteria is met
	{
		
		auto stop = chrono::steady_clock::now(); 
		auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
		if(duration.count() > timelimit - 2*((double)duration.count()/max(1,annealer.getEpochCount())))
		{
			cout << "Timelimit: " << timelimit << "\tElapsed: " << duration.count() << endl;
			cout << "\n\nEnding due to timelimit!\n\n";
			break;
		}

		annealer.performEpoch();
		/*
		//after each epoch, set to best solution found
		if(annealer.getEpochCount() % 5 == 0)
		{
			if(!annealer.resetToBest())
			{
				cout << "No better solutions found in 5 epochs! Reset to best...\n";
			}
		}
		else
		{
		//	annealer.resetToBest();
		}
		*/

		//after each epoch, display status
		if(print)
		{
			cout << "Kernel count: " << Kernel::getKernelCount() << "\t";
			annealer.printMoveCount();
			annealer.WSEcostFunction();
			annealer.printTemp();
			annealer.printCost();
			printPercentFilled();
			printTimestamp();
			printKernels();
			//printTimeAndArea();
		}
		updateVisual(NOPAUSE, annealer.getEpochCount());
		//updateVisual(PAUSE, annealer.getEpochCount());
	}

	//kernels = annealer.getBlocks();
	kernels = annealer.getBestBlocks();
	cout << "\n**********\n";
	cout << "EXIT CRITERIA MET..." << endl;
	cout << "Displaying best solution..." << endl;
	annealer.resetToBest();
	annealer.printResults();
	cout << "\n****END performAnnealing()****\n";
} //performAnnealing()

void CometPlacer::computePossibleKernels()
{
	for(unsigned int i = 0; i < kernels.size(); i++)
	{
		kernels[i]->computePossibleKernels();
	}
}

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
		
	printf("TIME ELAPSED: %.02fs (%.01fm)\t(%.02fs since previous)\n", total_time_elapsed, total_time_elapsed/60, time_since_previous);
	
	auto stop = chrono::steady_clock::now(); 
	auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
	cout << "chrono duration: " << duration.count() << "s" << endl; 
	timestamp = clock();
}

bool CometPlacer::legalizeLayout()
{
	return annealer.legalizeLayout();
}

int main(int argc, char** argv)
{
	cout << "****CometPlacer for WSE****\n\n";
	srand(time(0)); //set random seed
	
	string kgraph_filepath = "kgraph.txt"; //file to read kgraph input
	string output_filepath = "placement.paint"; //file to write output to

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

	//placer.printInfo();
	placer.updateVisual();
	//placer.printTimeAndArea();
//	placer.printKernels();

	placer.enforceMemoryConstraint();
//	placer.updateVisual(PAUSE); 
	placer.inflateKernelSize(0.8);
	placer.enforceMemoryConstraint();
//	placer.printKernels();
//	placer.printTimeAndArea();
//	placer.updateVisual(NOPAUSE); 

	placer.performAnnealing();
	//placer.updateVisual(true); 
	placer.printOutputToFile(output_filepath);
//	placer.printKernels();
//	placer.printTimeAndArea();
	placer.printInfo();
	placer.enforceMemoryConstraint();
//	placer.legalizeLayout();
	placer.updateVisual(PAUSE); 

	exit(0);
}
