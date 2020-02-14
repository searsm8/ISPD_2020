//Slicing_Annealer.h
//author: Mark Sears
//
//Slicing_Annealer take a group of blocks or similar 
//rectangles and performs Simulated Annealer heuristic
//to produce a layout using slicing method
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

	double max_width, max_height; //maximum size restrictions on the block layout

	vector<int> move_weights = {70, 20, 10}; //determines how often each type of move is done
	double reduction_factor = 0.97; //factor that reduces annealing temperature
	int steps_per_temp = 100; //number of steps before reducing temperature
	double temp; 	//current annealing temperature

	Block_Wrapper<Block>* best_layout;
	vector<Block*> best_blocks;
	vector<string> best_ops;
	double best_cost;

	public:

//CONSTRUCTORS

	Slicing_Annealer()
	{
	}


//ACCESSORS
	double getTemp() { return temp; }
//MODIFIERS
	
	void setBlocks(vector<Block*> new_blocks)
	{
		blocks = new_blocks;
	}

//ANNEALING FUNCTIONS

	//initial temperature should be high enough such that bad moves 
	//which are 3 std dev should be accepted with high probability
	void initializeTemp()
	{
		temp = 100;
	}

	//initial op strings can be all random, or uniform
	void initializeOps()
	{
		ops.push_back(""); //first op must always be empty

		for(int i = 1; i < blocks.size(); i++)
			ops.push_back("h");
	}



}; //end Slicing_Annealer

#endif

