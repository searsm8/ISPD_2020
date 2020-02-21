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

	vector < pair<int, int> > shapes; //pair<width, height>
		//set of possible shapes that are within A.R. limits
		//should be sorted by ascending widths
						
	double w_width, w_height; //wrapper width and height
	int x, y;

	bool print = true;

public:
//CONSTRUCTORS
	
	//Block_Wrapper() {} //default constructor should not be used?
	Block_Wrapper(Block* b)
	: base_level(true), base_block(b), x(0), y(0)
	{
		setShapesBaseBlock();
	}
	
	
	Block_Wrapper(Block_Wrapper* a, Block_Wrapper* b, char initCut='h')
	: base_level(false), w1(a), w2(b), cut(initCut), x(0), y(0)
	{
		combineShapes();
	}
	
//ACCESSORS
	
	double getWidth()  { return w_width; }
	
	double getHeight()  { return w_height; }

	int getX() { return x; }

	int getY() { return y; }
	
	double getArea()  { return getWidth() * getHeight(); }
	
vector < pair<float, float> > getShapes() { return shapes; }
	
//MODIFIERS
	void setCut(char new_cut) { cut = new_cut; } 
	
	void setX(int new_x) { x = new_x; }

	void setY(int new_y) { y = new_y; }
//COMPUTATORS
	
	//based on the cut type, set the dimensions of this wrapper
	void updateDimensionsBaseBlock()
	{
		//for base level wrappers, the dimensions are equal to the block it contains
		w_width = base_block->getWidth();
		w_height= base_block->getHeight();

		//set the block x and y
		base_block->setX(x);
		base_block->setY(y);

	}

	//set the shapes equal to the kernel dimensions in
	void setShapesBaseBlock()
	{	
		vector<Block*> possible_blocks = base_block->getPossibleKernels();

		shapes.clear();
		for(int i = 0; i < possible_blocks.size(); i++)
		{
			shapes.push_back(pair<int, int>(possible_blocks[i]->getWidth(), possible_blocks[i]->getHeight()));
		}
			//TODO add shapes to the base block for 90 degree rotations!


		printShapes();
	}

	void updateDimensions()
	{
		if(base_level)
			updateDimensionsBaseBlock();

		//for non-base level wrappers, dimensions are the combination of w1 and w2,
		//depending on the cut type
		else if(cut == 'h')
		{
			w_width = max(w1->getWidth(), w2->getWidth());
			w_height = w1->getHeight() + w2->getHeight();
		}
		else //if not 'h', then assume cut == 'v'
		{
			w_height = max(w1->getHeight(), w2->getHeight());
			w_width = w1->getWidth() + w2->getWidth();
		}
	}

	void updatePosition()
	{
		if(base_level)
			return;

		//for non-base level wrappers, dimensions are the combination of w1 and w2,
		//depending on the cut type
		else if(cut == 'h')
		{
			//set the block x and y
			w1->setX(x);
			w1->setY(y);
			w2->setX(x);
			w2->setY(y + w1->getHeight());
		}
		else //if not 'h', then assume cut == 'v'
		{
			//set the block x and y
			w1->setX(x);
			w1->setY(y);
			w2->setX(x + w1->getWidth());
			w2->setY(y);
		}

		//next, update the positions for all blocks inside this wrapper
		w1->updatePosition();
		w2->updatePosition();
	}
	
	//combine the shapes of the two inner wrappers, based on cut type
	void combineShapes()
	{	
		if(base_level)
			return;

		int index_1 = 0;
		int index_2 = 0;

		shapes.clear();
		pair<int, int> new_shape;

		if(cut == 'h')
		{
			while(index_1 < w1->shapes.size() && index_2 < w2->shapes.size())
			{
				//for horizontal cut, add heights together.
				new_shape.second = w1->shapes[index_1].second + w2->shapes[index_2].second;
				if(w1->shapes[index_1].first > w2->shapes[index_2].first)
					new_shape.first = w1->shapes[index_1++].first;
				else
					new_shape.first = w2->shapes[index_2++].first;
				
				if(shapes.size() == 0 || shapes.back().first > new_shape.first)
					shapes.push_back(new_shape);
			}
		}
		else //it's a 'v' cut
		{
			index_1 = w1->shapes.size()-1;
			index_2 = w2->shapes.size()-1;

			while(index_2 >= 0 && index_2 >= 0)
			{
				//for vertical cut, add widths together.
				new_shape.first= w1->shapes[index_1].first + w2->shapes[index_2].first;
				if(w1->shapes[index_1].second > w2->shapes[index_2].second)
					new_shape.second = w1->shapes[index_1--].second;
				else
					new_shape.second = w2->shapes[index_2--].second;

				if(shapes.size() == 0 || shapes.front().second > new_shape.second)
					shapes.insert(shapes.begin(), new_shape); //must keep the shapes sorted in descending widths
			}
		}

		printShapes();
	}

//PRINTERS
	void printShapes()
	{
		if(!print) return;
		cout << "\nBlock_Wrapper shapes:\t"; 
		if(base_block)
			cout << "(Base Block)\n";
		else cout << "(" << cut << " cut)\n";
		for(int i = 0; i < shapes.size(); i++)
		{
			cout << "(" << shapes[i].first << ", " << shapes[i].second << ")\n";
		}
	}

}; //end Block_Wrapper

#endif


