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
#include <stack>
#include <random>
#include <time.h>

#include "util.h"


using namespace std;

template<class Block>//cost function?
class Slicing_Annealer
{
private:
	vector<Block*> blocks;
	vector<string> ops;	 //string of "h" horizontal cut and
				//"v" vertical cuts
	Block* head;

	int wirepenalty;
	double WSE_width, WSE_height; //maximum size restrictions on the block layout

	vector<int> move_weights; //determines how often each type of move is done
	double reduction_factor; //factor that reduces annealing temperature
	int steps_per_temp; //number of steps before reducing temperature
	double temp; 	//current annealing temperature

	Block_Wrapper<Block>* layout;
	Block_Wrapper<Block>* prev_layout;
	double prev_cost;
	double best_cost;
	double best_wirelen;

	vector<Block*> best_blocks;
	vector<string> best_ops;
	vector<Block*> prev_blocks;
	vector<string> prev_ops;
	vector<int> previous_ops_count;
	vector< vector<int> > next_block_indices;

	int move_count;
	vector<int> move_indices; //track the index that each move is performed at
	int prev_move_num;
	int reject_count;
	int MIN_TEMP;
	int epoch_count;
	int anneal_phase;

public:

//CONSTRUCTORS

	Slicing_Annealer() {}

	Slicing_Annealer(int init_wirepenalty, int init_width, int init_height) 
	:wirepenalty(init_wirepenalty), WSE_width(init_width), WSE_height(init_height), reduction_factor(.9), steps_per_temp(10000), temp(0), prev_cost(0),best_cost(-1), move_count(0), prev_move_num(0), reject_count(0), MIN_TEMP(10), epoch_count(0), anneal_phase(0)
	{
		move_weights = {70, 20, 10, 0, 0}; //determines how often each type of move is done
		move_indices = {0, 0, 0, 0, 0}; 
	}


//ACCESSORS

	int getEpochCount() { return epoch_count; }

	int getMoveCount() { return move_count; }

	double getTemp() { return temp; }

	int getStepsPerTemp() { return steps_per_temp; }

	int getLayoutWidth() { return layout->getWidth(); }

	int getLayoutHeight() { return layout->getHeight(); }

	vector<Block*> getBlocks() { return blocks; }

	vector<Block*> getBestBlocks()
       	{	      
	       	return best_blocks;
       	}

	int getBestCost() { return best_cost; }

//MODIFIERS
	
	void setBlocks(vector<Block*> new_blocks)
	{
		blocks = new_blocks;
		//prev_blocks = new_blocks;
	}

	//set the head block, and update the block order based on connections
	void setHead(Block* new_head)
	{
		head = new_head;

		//order the blocks such that head is first, followed by connected blocks
		blocks.clear();
		addToBlocks(head);
	}

	void addToBlocks(Block* b)
	{
		blocks.push_back(b);

		for(unsigned int i = 0; i < b->next_kernels.size(); i++)
			addToBlocks(b->next_kernels[i]);
	}

//PRINT FUNCTIONS

	void printOps()
	{
		if(!print) return;
		cout << "Slicing ops: ";
		for(unsigned int i = 0; i < ops.size(); i++)
			cout << blocks[i]->getName() << " " << ops[i] << " ";
		cout << endl;
	}

	void printTemp()
	{
		if(!print) return;
		cout << "Temp: " << temp << "\treduction_factor: " << reduction_factor << endl;
	}

	void printCost()
	{
		if(!print) return;
		cout << "Prev Cost: " << prev_cost << "\tBest Cost: " << best_cost << "\tBest Wirelen: " << best_wirelen << endl;
	}

	void printMoveCount()
	{
		if(!print) return;
		cout << "Move Count: " << move_count << endl;
	}


	void printResults()
	{
		cout << "\n******RESULTS******\n";
		cout << "Final Cost: " << costFunction() << endl;

	}


//ANNEALING FUNCTIONS

	//initial op strings can be all random, or uniform
	void initializeOps()
	{
		ops.push_back(""); //first op must always be empty

		for(unsigned int i = 1; i < blocks.size(); i++)
			ops.push_back("h");

		while(previous_ops_count.size() < ops.size())
			previous_ops_count.push_back(0);
//cout << "blocks.size() = " << blocks.size() << endl;
//cout << "ops.size() = " << ops.size() << endl;
			
		for(unsigned int i = 0; i < blocks.size(); i++)
		{
			prev_blocks.push_back(blocks[i]->createCopy());
			prev_ops.push_back(ops[i]);
			best_blocks.push_back(blocks[i]->createCopy());
			best_ops.push_back(ops[i]);
		}

	}


	//initial temperature should be high enough such that bad moves 
	//which are 3 std dev should be accepted with high probability
	//perform many random moves, record deltas
	//set starting temperature equal to 30*std_dev
	float initializeTemp()
	{
		for(unsigned int i = 0; i < blocks.size(); i++)
			blocks[i]->computePossibleKernels();

		layout = findBestLayout();
		prev_layout = layout;
		
		resetRejectCount();
		prev_cost = costFunction();

		temp = 10000;	return temp;
///////
		int num_initialize_moves = blocks.size()*100;

		printf("Initializing temp...(%d random moves)\n", 2*num_initialize_moves);
		vector <double> deltas;
		float new_cost, delta;
			
		//perform random cell swaps to obtain a randomized layout
		for(int i = 0; i < num_initialize_moves; i++)
			performMove(); //random move with no rejection
			
		//perform random cell swaps and record deltas	
		for(int i = 0; i < num_initialize_moves; i++)
		{
			performMove(); //random move with no rejection
			delete layout;
			layout = findBestLayout();
					
			new_cost = costFunction();
		
			delta = new_cost - prev_cost;
#ifdef DEBUG
	cout << "delta: " << delta << endl;
#endif
			deltas.push_back(delta);
			
			acceptMove(new_cost); //always accept the move during initialization	
		}
		
		prev_layout = layout;

		double std_dev = StandardDeviation(deltas);
			
		temp = 1*std_dev; //for a temperature of 30*std_dev,
				//there will be a 90% chance to accept a very bad change of 3*std_dev
		cout << "\n\n************************\n";
		cout << "std_dev: " << std_dev << endl;		
		cout << "Starting Temp: " << temp << endl;
		cout << "************************\n";
		
		prev_cost = costFunction();
		resetRejectCount();

		return temp;
	}

	void performEpoch()
	{
		cout << "\n\n****BEGIN EPOCH #" << ++epoch_count << endl;
		resetRejectCount();
		for(int i = 0; i < getStepsPerTemp(); i++)
			performAnnealingStep();

		reduceTemp();
	}

	bool performAnnealingStep()
	{
		performMove();

		//return true if step was performed
		//segfault in evaluateMove!
		return evaluateMove();
	//	return true;
	}

	void reduceTemp()
	{
		//control the temperature schedule

		if(temp < 10000)
			reduction_factor = .9825;
		else if(temp < 50000)
			reduction_factor = .975;
		else if(temp < 100000)
			reduction_factor = .95;
		else
			reduction_factor = .90;

		temp *= reduction_factor;
	}
	
	//TODO add more moves!
	//move4() change a larger block orientation (how???)
	void performMove()
	{	
		prev_move_num = 0;
		switch(pickMove())
		{
			case 1: move1(); break;
			case 2: move2(); break;
			case 3: move3(); break;
			case 4: move4(); break;
			case 5: move5(); break;
			default:move1();
		}

		++move_count;
	}

	//returns a number corresponding to a move picked randomly based on weights
	//e.g. weight1 = 50, weight2 = 25, weight3 = 25
	//will pick move1 at 50%, move2 at 25%
	int pickMove()
	{
		int sum_of_weights = 0;
		for(unsigned int i = 0; i < move_weights.size(); i++)
			sum_of_weights += move_weights[i];
			
		int pick = rand() % sum_of_weights;
		
		for(unsigned int i = 0; i < move_weights.size(); i++)
		{
			if(pick < move_weights[i])
				return i+1;
			pick -= move_weights[i];
		}

		return pick;
	}

	//move M1
	//swaps two random adjacent blocks
	void move1()
	{
#ifdef DEBUG
		cout << "move1()" << endl;
#endif

		move_indices[1] = rand() % (blocks.size()-1);
		prev_move_num = 1;
	
		Block* temp = blocks[move_indices[1]];
		blocks[move_indices[1]] = blocks[move_indices[1]+1];
		blocks[move_indices[1]+1] = temp;
	}

	void undoMove1()
	{
		Block* temp = blocks[move_indices[1]];
		blocks[move_indices[1]] = blocks[move_indices[1]+1];
		blocks[move_indices[1]+1] = temp;
	}

	//move M2
	//complements a random operator chain
	void move2()
	{
#ifdef DEBUG
		cout << "move2()" << endl;
#endif
		//choose a random op string until a non-empty one is found
		unsigned int i = rand() % (ops.size()-1);
		while(ops[i].size() == 0)
			i = rand() % (ops.size()-1);
	
		//complement the h's and v's in the string
		string complement = "";
		for(unsigned int j = 0; j < ops[i].size(); j++)
		{
			if(ops[i][j] == 'h')
				complement += 'v';
			else complement += 'h';
		}
		
		ops[i] = complement;
	}

	//move M3
	//swaps a random cell with an adjacent operator
	//must maintain normalized Polish expression and balloting property
	//pick a random op. Try to move it forward or backward.
	//If both are illegal moves, pick another random op 
	void move3()
	{
#ifdef DEBUG
		cout << "move3()" << endl;
#endif
		updatePreviousOpsCount(); //ensure that the op counts are correct!
		
		//attempt to swap a cell and operator. If move is not legal, pick another random op string
		int attempts = 0; 		
		while(attempts++ < (signed int)ops.size()) //try this many times, then give up and cancel move
		{
			//choose a random op string until a non-empty one is found
			int i = rand() % (ops.size()-1);
			while(ops[i].size() == 0)
				i = rand() % (ops.size()-1);				
			
			//choose direction to bump operation: forward or backward
			
			if(previous_ops_count[i-1] < i-1) //check if this move would preserve balloting
		  	{ 
				if(ops[i-1].size() == 0 || ops[i][0] != ops[i-1][ops[i-1].size()-1]) //check if this move would preserve normalization
				{//	printf("moving backward!\n");
					ops[i-1].push_back(ops[i][0]);
					ops[i].erase(ops[i].begin());
					break;			
				}
			} 
			
			if(i != (signed int)(ops.size() - 1)) //can't move the last op forward!
			{//	printf("moving forward!\n");		
				if(ops[i][ops[i].size()-1] != ops[i+1][0]) //check if this move would preserve normalization
				{
					ops[i+1].insert(0, 1, ops[i][ops[i].size()-1]);
					ops[i].erase(--ops[i].end());
					break;		
				} 
			}
		}	
		
		return;
	} //end move3

	//increase or decrease kernel size!
	void move4()
	{
#ifdef DEBUG
		cout << "move4()" << endl;
#endif
		//if(computeTotalWirelength() * wirepenalty < getLongestTime())
		if(rand()%2 == 0)
//		if(true)
		{
//			Block* b = getLongestBlock();
			for(unsigned int i = 0; i < blocks.size(); i++)
			{
				Block* b = getLongestBlock();
#ifdef DEBUG
	cout << "Increasing Block: " << b->getName() << endl;
#endif
				b->printPerformance();
				b->increaseSize();
				b->computePerformance();
				b->printPerformance();
				b->printParameters();
			}
		}
		else
		{
			//decrease size of a random block
		//	Block* b = blocks[rand()%blocks.size()];
			for(unsigned int i = 0; i < blocks.size(); i++)
			{
				Block* b = getShortestBlock();
				b->decreaseSize();
				b->computePerformance();
			}
			return;
		}
	} //end move4()

	//give a block a new random AR and orientation
	void move5()
	{
#ifdef DEBUG
		cout << "move5()" << endl;
#endif
		random_device rd("default");
		mt19937 gen{rd()};
		
		double mean = 1.5, std_dev = .3;
		normal_distribution<double> AR_distribution{mean, std_dev};
		double new_AR = AR_distribution(gen);

		if(new_AR < 0) new_AR = 1;

		if(rand()%2 == 0)
			new_AR = 1 / new_AR;

		//apply the new AR to a random kernel
		blocks[rand()%blocks.size()]->changeShapeToAR(new_AR);
			
	} //end move5()


	void updatePreviousOpsCount()
	{
		for(unsigned int i = 1; i < ops.size(); i++)	
			previous_ops_count[i] = previous_ops_count[i-1] + ops[i].size();

#ifdef DEBUG
		cout << "ops_count: ";
		for(unsigned int i = 0; i < previous_ops_count.size(); i++)	
			cout << previous_ops_count[i] << " ";
		cout << endl; 
#endif
	}

	//evaluateMove compares the new layout's area and wirelength to the previous.
	//then accepts or rejects the move 
	//return true if a move was made
	bool evaluateMove()
	{
		delete layout; //delete previous layout to prevent memory leak
		layout = findBestLayout();	
		double new_cost = costFunction();
		double delta = new_cost - prev_cost;

#ifdef DEBUG
		cout << "Layout dims: (" << layout->getWidth() <<", " << layout->getHeight() << ")" <<endl;
		cout << "Layout Area: " << layout->getArea() << endl;
		cout << "Temp: " << temp << endl;
		cout << "Cost difference: " << new_cost-prev_cost << endl;
#endif
		//check if the new layout is better
		if(delta <= 0)
		{
			acceptMove(new_cost);
			return true;
		}
		else //else no imrovement was made. keep the new layout conditionally
		{
			//compute the probability to accept the "bad" move
			double Pr_accepting = exp((double)(-1*delta)/(double)temp);
				
	//		if(rand() > RAND_MAX*Pr_rejecting)
			int p = 100000;		
			float roll = (float)(rand()%p) / (float)p; 

#ifdef DEBUG
	cout << "Pr_accepting : " << Pr_accepting;
	cout << "\t roll: " << roll << endl;
#endif

			if(roll < Pr_accepting) //chance to accept or reject
			{
				acceptMove(new_cost);
				return true;
			}
			else
			{
				rejectMove();
				return false;
			}
				
		}

		return true;
	}

	int accept_count = 0;

	void acceptMove(double new_cost)
	{
#ifdef DEBUG
		cout << "***acceptMove()\n\n";
#endif
		//if not an improvement, still make the change,
		//but count it as a reject for equilibrium measurment
		//if(new_cost >= prev_cost)
		//	reject_count++;
		//else 
			accept_count++;
	//	if(prev_move_num == 1)
	//		return;


		for(unsigned int i = 0; i < blocks.size(); i++)
		{
			prev_blocks[i]->copyDataFrom(blocks[i]);
			prev_ops[i] = string(ops[i]);
		}
		prev_cost = new_cost;

		//if a new best cost has been found, update blocks and variables
		if(new_cost < best_cost || best_cost == -1)
		{
			best_cost = new_cost;
			best_wirelen = computeTotalWirelength();
			updateBest();
			/*
			for(unsigned int i = 0; i < blocks.size(); i++)
			{
				best_blocks[i]->copyDataFrom(blocks[i]);
				best_ops[i] = string(ops[i]);
			}
			*/
		}
		//delete prev_layout; //delete previous layout to prevent memory leak
		//prev_layout = layout;// new Block_Wrapper<Kernel>(*layout);
	}

	void rejectMove()
	{
#ifdef DEBUG
		cout << "***rejectMove()\n\n";
#endif
		reject_count++;
		
		//TODO add undoMove for other moves
		if(prev_move_num == 1)
		{
			undoMove1();
			return;
		}

		else
		{
			for(unsigned int i = 0; i < prev_blocks.size(); i++)
			{
				blocks[i]->copyDataFrom(prev_blocks[i]);
				ops[i] = string(prev_ops[i]);
			}
		}

		delete layout; //delete previous layout to prevent memory leak
		layout = new Block_Wrapper<Kernel>(*findBestLayout());

	}

	int update_best_count = 0;
	void updateBest()
	{
		update_best_count++;
		best_blocks.clear();

		for(unsigned int i = 0; i < blocks.size(); i++)
		{
			//ops always maintain the same order
			best_ops[i] = string(ops[i]);
			best_blocks.push_back(blocks[i]);
/*
			//since kernels are swapped around, must match to ID
			for(unsigned int j = 0; j < best_blocks.size(); j++)
			{
				if(blocks[i]->ID == best_blocks[j]->ID)
				{
					temp_blocks;
					//best_blocks[i]= blocks[j]->createCopy();
					break;
				}
			}
			*/
		}
	}

	void resetToBest()
	{
		cout << "update_best_count: " << update_best_count << endl;
		update_best_count = 0;

		blocks.clear();
		for(unsigned int i = 0; i < best_blocks.size(); i++)
		{
			//ops always maintain the same order
			ops[i] = string(best_ops[i]);
			blocks.push_back(best_blocks[i]);

			/*
			//since kernels are swapped around, must match to ID
			for(unsigned int j = 0; j < best_blocks.size(); j++)
			{
				if(blocks[i]->ID == best_blocks[j]->ID)
				{
					//make a copy! 
					blocks[i]->copyDataFrom(best_blocks[j]);
					//blocks[i] = best_blocks[j]->createCopy();
					break;
				}
			}
			*/
		}

		//TODO make sure that updating best blocks works! and doesn't mess up final result

		for(unsigned int i = 0; i < blocks.size(); i++)
		{
			cout << blocks[i]->getName() << " " << blocks[i]->ID << " " << ops[i] << endl;
		}
			cout << endl;
		delete layout; //delete previous layout to prevent memory leak
		layout = new Block_Wrapper<Kernel>(*findBestLayout());
	}

	//using the ops slicing string, combine blocks until a full layout is achieved
	//finds the minimum possible area given the blocks and ops string
	Block_Wrapper<Block>* findBestLayout()
	{
		//use a stack to read the layout string
		stack<Block_Wrapper<Block>*> block_stack;

		//combine all blocks based on the ops chain
		for(unsigned int i = 0; i < blocks.size(); i++)
		{
			blocks[i]->computePerformance();
			//put the next cell in a wrapper, then on the stack
			block_stack.push(new Block_Wrapper<Block>(blocks[i]));

			//resolve all relevant h or v cut operators
			for(unsigned int op_index = 0; op_index < ops[i].size(); op_index++)
			{
				//pop the 2 blocks on top and combine them
			 	Block_Wrapper<Block>* b1 = block_stack.top();
				block_stack.pop();
				Block_Wrapper<Block>* b2 = block_stack.top();
				block_stack.pop();
				
				//combine the two blocks based on the operator
				//then put the new combined block back on the stack
				block_stack.push(new Block_Wrapper<Block>(b1, b2, ops[i][op_index]));
		 	}
		}

		//find the best area for the shapes in the layout
		Block_Wrapper<Block>* new_layout = block_stack.top();
		int prev_area = new_layout->getShapeArea(0);
		int prev_index = 0;

		for(unsigned int i = 1; i < new_layout->getShapes().size(); i++)
		{
			int next_area = new_layout->getShapeArea(i); 
			if(next_area < prev_area)
			{
				prev_area = next_area;	
				prev_index = i;
			}
		}
			
		//given the best area found for the layout,
		//solidify the dimensions and positions of 
		//all Blocks
		new_layout->solidifyShape(prev_index);
		new_layout->updatePosition();
		new_layout->updateDimensions();

		return new_layout;

	}

	void updateBlocks()
	{
	//	findBestLayout();
		layout->updateDimensions();	
	}

	double computeTotalWirelength()
	{
		double wirelen = 0;

		for(Block* b : blocks)
			wirelen += b->computeWirelength();

		return wirelen;
	}

	//compute a cost function to be used in the annealing process
	double costFunction()
	{
		//weights
		//double alpha = 3.0;

		//compute wire distance cost
		int wirelen = computeTotalWirelength();

		int longest_time = getLongestTime();

//		double cost = longest_time + wirelen*wirepenalty + alpha*layout->getArea();
		double cost = longest_time + wirelen*wirepenalty;
		//impose a penalty for shapes that aren't within the allowed area! 
		cost *= max(1.0, pow(layout->getWidth() / WSE_width, anneal_phase));
		cost *= max(1.0, pow(layout->getHeight() / WSE_height, anneal_phase));
#ifdef DEBUG
cout << "Wirelength: " << wirelen << "*" << wirepenalty << " = "<< wirelen*wirepenalty << endl;
cout << "Longest Time: " << longest_time << endl;
cout << "Total cost: " << cost << " (Prev Cost: " << prev_cost << ")" <<  endl;
#endif
		return cost;
	}

	//only count the wirelength and longest kernel time.
	//NOT USED FOR ANNEALING PROCESS
	double WSEcostFunction()
	{
		layout->updateDimensions(); //must set x and y coordinates of each block before estimating wirelen!
		int wirelen = computeTotalWirelength();

		int longest_time = getLongestTime();

		double cost = longest_time + wirelen*wirepenalty;

		cout << "WSE competition cost: " << cost << "\twirelength: " << wirelen << "\tTime: " << longest_time << endl;
		return cost;
	}
	
	int getLongestTime()
	{
		return getLongestBlock()->getTime();
	}

	Block* getLongestBlock()
	{
		Block* b = blocks[0];
		int longest_time = b->getTime();
		for(unsigned int i = 1; i < blocks.size(); i++)
			if(blocks[i]->getTime() > longest_time)
			{
				b = blocks[i];
				longest_time = b->getTime();
			}
		return b;
	}

	Block* getShortestBlock()
	{
		Block* b = blocks[0];
		int shortest_time = b->getTime();
		for(unsigned int i = 1; i < blocks.size(); i++)
			if(blocks[i]->getTime() < shortest_time)
			{
				b = blocks[i];
				shortest_time = b->getTime();
			}
		return b;
	}

	void resetRejectCount()
	{
		accept_count = 0;
		reject_count = 0;
	}

	//return true if "equilibrium" is reached,
	//when no new moves have been accepted for many moves
	bool equilibriumReached()
	{
		cout << "reject_count: " << reject_count << "\taccept_count: " << accept_count << "\tsteps_per_temp: " << steps_per_temp << endl;
		//consider equilibrium reached if at least 99% of moves are rejected
		if(reject_count >= steps_per_temp*0.99)
			return true;
		//or stop if reach a small enough temperature
		else if(temp < MIN_TEMP)
			return true;
		else return false;
	}

}; //end Slicing_Annealer

#endif

