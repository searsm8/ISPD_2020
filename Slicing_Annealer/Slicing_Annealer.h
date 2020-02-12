//Slicing_Annealer.h
//author: Mark Sears
//
//Slicing_Annealer take a group of blocks or similar 
//rectangles and performs Simulated Annealer heuristic
//to produce a layout
//

#ifndef _SLICING_ANNEALER
#define _SLICING_ANNEALER

#include "Block_Wrapper.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

template<class Block>//cost function?
class Slicing_Annealer
{
	private:
	vector<Block*> blocks;
	vector<string> ops;	 //string of "h" horizontal cut and
				//"v" vertical cuts

	vector<int> move_weights = {70, 20, 10};

	double temp; 	//current annealing temperature
		
	public:

//CONSTRUCTORS
	Slicing_Annealer(vector<Block*> initBlocks)
	: blocks(initBlocks) //initialization
	{}

//ACCESSORS
	double getTemp() { return temp; }
//MODIFIERS
//ANNEALING FUNCTIONS

}; //end Slicing_Annealer

#endif

