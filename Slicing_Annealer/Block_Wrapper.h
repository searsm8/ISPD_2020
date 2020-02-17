//Block_Wrapper.h
//
//A Wrapper is a simple structure which helps organize Blocks for slicing Simulated Annealing
//It contains either 1 Block or 2 other Block Wrappers
//(Each Block might in turn contain other Blocks, such as Dblock or Cblock)
//If the Block Wrapper contains a single Block, it is a "base level" wrapper
//If it contains 2 other Wrappers, it is somewhere in the hierarchy that makes up the layout
//and has either a V-cut or H-cut seperating the two Blocks
//
//Finally, the top level Block_Wrapper will contain all blocks in the layout
//and will *hopefully* fit on the WSE!

#ifndef _BLOCK_WRAPPER
#define _BLOCK_WRAPPER

#include <algorithm>

using namespace std;

//whatever class is passed as a "Block" must implement functions:
//getWidth()
//getHeight()
template <class Block>
class Block_Wrapper
{
private:
	bool base_level;
	char cut; //'h' for horizontal cut, or 'v' for vertical cut

       	//if the Wrapper is base level, then it contains a single Block
	Block* base_block;

	//otherwise, it will contain two other Wrappers
	Block_Wrapper* w1;
	Block_Wrapper* w2;

	double w_width, w_height; //wrapper width and height
	int x, y;

public:
//CONSTRUCTORS
	
	//Block_Wrapper() {} //default constructor should not be used?
	Block_Wrapper(Block* b)
	: base_level(true), base_block(b), x(0), y(0)
	{
		updateDimensions();
	}
	
	
	Block_Wrapper(Block_Wrapper* a, Block_Wrapper* b, char initCut='h')
	: base_level(false), w1(a), w2(b), cut(initCut), x(0), y(0)
	{
		updateDimensions();
	}
	
//ACCESSORS
	
	double getWidth()  { return w_width; }
	
	double getHeight()  { return w_height; }

	int getX() { return x; }

	int getY() { return y; }
	
	double getArea()  { return getWidth() * getHeight(); }
	
	
//MODIFIERS
	void setCut(char new_cut) { cut = new_cut; } 
	
	void setX(int new_x) { x = new_x; }

	void setY(int new_y) { y = new_y; }
//COMPUTATORS
	
	//based on the cut type, set the dimensions of this wrapper
	void updateDimensions()
	{
		if(base_level)
		{
			//for base level wrappers, the dimensions are equal to the block it contains
			w_width = base_block->getWidth();
			w_height= base_block->getHeight();

			//set the block x and y
			base_block->setX(x);
			base_block->setY(y);

		}
		else
		{
			//for non-base level wrappers, dimensions are the combination of w1 and w2,
			//depending on the cut type
			if(cut == 'h')
			{
				w_width = max(w1->getWidth(), w2->getWidth());
				w_height = w1->getHeight() + w2->getHeight();
				//set the block x and y
				w1->setX(x);
				w1->setY(y);
				w2->setX(x);
				w2->setY(y + w1->getHeight());
			}
			else //if not 'h', then assume cut == 'v'
			{
				w_height = max(w1->getHeight(), w2->getHeight());
				w_width = w1->getWidth() + w2->getWidth();
				//set the block x and y
				w1->setX(x);
				w1->setY(y);
				w2->setX(x + w1->getWidth());
				w2->setY(y);
			}

			//next, update the dimensions for all blocks inside this wrapper
			w1->updateDimensions();
			w2->updateDimensions();
		}
	}
}; //end Block_Wrapper

#endif


