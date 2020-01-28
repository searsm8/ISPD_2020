//CometPlacer.h
//author: Mark Sears
//
//Defines CometPlacer class, which manipulates kernels 
//in the WSE to optimize performance
#ifndef _CometPlacer
#define _CometPlacer

#include "kernel/Kernels.h"
#include "visual/VisualizeWSE.h"

class CometPlacer
{
private:
	int VISUAL_UPDATE_INTERVAL = 3000;
	string output;
	int wirepenalty;
	int timelimit;
	int width;
	int height;

	vector<Kernel*> kernels;
	int iteration;
	SDL_Window* window;

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
		window = createWSE(width, height);
	
	}

	void readKgraph(string kgraph_filename)
	{
	Dblock* k1 = new Dblock(56, 56, 256, 4, 4, 8, 8, 8, 8, 16, 16);
	k1->computePerformance();
	k1->setX(200);
	k1->setY(200);

	Cblock* k2 = new Cblock(56, 56, 512, 4, 4, 8, 8, 8, 8, 8, 32, 32, 19);
	k2->computePerformance();
	k2->setX(150);
	k2->setY(0);
	
	Dblock* k3 = new Dblock(28, 28, 256, 4, 4, 8, 8, 8, 8, 32, 8);
	k3->computePerformance();
       	k3->setX(250);
	k3->setY(0);

	kernels.push_back(k1);
//	kernels.push_back(k2);
//	kernels.push_back(k3);
		
	k1->printPerformance();
	}

	//print general info about the WSE layout
	void printInfo()
	{
		cout << "\n****WSE INFO****\n";
		cout << "(" << width << ", " << height << ")\n";
		cout << "#Kernels: " << kernels.size() << endl;
		cout << "Iteration: " << iteration << endl; 
		cout << "wirepenalty: " << wirepenalty << endl; 
		cout << "timelimit: " << timelimit << endl; 

	}

	//visualize the kernels on the WSE
	void updateVisual()
	{
		updateWSE(window, kernels, iteration);
		SDL_Delay(VISUAL_UPDATE_INTERVAL);	
	}


//MODIFIERS

	void modifyKernel(int kernel_num, string key, int new_val)
	{
		Kernel* k = kernels[kernel_num];
		cout << endl;
		cout << "modifyKernel("<<kernel_num<<", "<<key<<","<<new_val<<")" <<endl;
		k->setEP(key, new_val);
		k->computePerformance();
		k->update();
		k->printPerformance();
		cout << endl;
	}

};
#endif

