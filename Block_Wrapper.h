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
#include "util.h"

using namespace std;

//whatever class is passed as a "Block" must implement functions:
//getWidth()
//getHeight()
extern int WSE_width, WSE_height;

template <class Block>
class Block_Wrapper
{
private:
	bool base_level;

       	//if the Wrapper is base level, then it contains a single Block
	Block* base_block;

	//otherwise, it will contain two other Wrappers
	Block_Wrapper* w1;
	Block_Wrapper* w2;
	char cut; //'h' for horizontal cut, or 'v' for vertical cut

	vector < pair<int, int> > shapes; //pair<width, height>
		//set of possible shapes that are within A.R. limits
		//should be sorted by DESCENDING widths
						
	double w_width, w_height; //wrapper width and height
	int x, y;

	bool print = true;

public:
//CONSTRUCTORS
	
	//Block_Wrapper() {} //default constructor should not be used?
//constructor for base level wrapper
	Block_Wrapper(Block* b)
	: base_level(true), base_block(b), x(0), y(0)
	{
		setShapesBaseBlock();
	}
	
	
//constructor for non-base level wrapper
	Block_Wrapper(Block_Wrapper* a, Block_Wrapper* b, char initCut='h')
	: base_level(false), w1(a), w2(b), cut(initCut), x(0), y(0)
	{
		combineShapes();
	}

/*DESTRUCTOR
 *the block wrapper should recursively destroy all lower level
 *wrappers below it in order to prevent a memory leak
 */
	~Block_Wrapper()
	{

		if(!base_level)
		{
			delete w1;
			delete w2;
		}
	}

	
//ACCESSORS
	
	double getWidth()  { return w_width; }
	
	double getHeight()  { return w_height; }

	int getX() { return x; }

	int getY() { return y; }
	
	double getArea()  { return getWidth() * getHeight(); }
	
vector < pair<int, int> > getShapes() { return shapes; }
	
//MODIFIERS
	void setCut(char new_cut) { cut = new_cut; } 
	
	void setX(int new_x) { x = new_x; }

	void setY(int new_y) { y = new_y; }

	void setWidth(int new_width) { w_width = new_width; }

	void setHeight(int new_height) { w_height = new_height; }

//COMPUTATORS
	
	//based on the cut type, set the dimensions of this wrapper
	void updateDimensionsBaseBlock()
	{
		//for base level wrappers, the dimensions are equal to the block it contains
		w_width = base_block->getWidth();
		w_height= base_block->getHeight();
	}

	//set the shapes equal to the kernel dimensions in
	void setShapesBaseBlock()
	{	
		vector<Block*> possible_blocks = base_block->getPossibleKernels();

		shapes.clear();

		//shapes.push_back(pair<int, int>(possible_blocks[possible_blocks.size()-1]->getWidth(), possible_blocks[possible_blocks.size()-1]->getWidth()));
		//shapes.push_back(pair<int, int>(possible_blocks[0]->getWidth(), possible_blocks[0]->getWidth()));
		//return;
			
		for(unsigned int i = 0; i < possible_blocks.size(); i++)
			shapes.push_back(pair<int, int>(possible_blocks[i]->getWidth(), possible_blocks[i]->getHeight()));

			//add shapes to the base block for 90 degree rotations!
		for(int i = possible_blocks.size()-2; i >= 0; i--)
			shapes.push_back(pair<int, int>(possible_blocks[i]->getHeight(), possible_blocks[i]->getWidth()));


		//printShapes();
	}

	void updateDimensions()
	{
//cout << "updateDimensions()\n";
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
//cout << "updatePosition(): (" << x << ", " << y << ")\n";
		//recursive base case
		if(base_level)
		{
//cout << "base_level\n";
			base_block->setX(x);
			base_block->setY(y);
			return;
		}

		//else, for non-base level wrappers, dimensions are the combination of w1 and w2,
		//depending on the cut type
		if(cut == 'h')
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
			while(index_1 < (signed int)w1->shapes.size() && index_2 < (signed int)w2->shapes.size())
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

			while(index_1 >= 0 && index_2 >= 0)
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

		//printShapes();
	} //end combineShapes()

	int getShapeArea(int index)
	{
		int area = shapes[index].first * shapes[index].second;	
		
		//return area;

		//add a factor that penalizes large or small aspect ratios
		double penalty = max((double)shapes[index].first/(double)shapes[index].second, (double)shapes[index].second/(double)shapes[index].first);
//		cout << "penalty = " << penalty << endl;
		double weighted_area = area * pow(penalty, 3);
		
		//put a hard limit on width and height
		if(shapes[index].first > WSE_width)
			weighted_area *= pow((double)(shapes[index].first)/(double)WSE_width, 3);
		if(shapes[index].second > WSE_height)
			weighted_area *= pow((double)(shapes[index].second)/(double)WSE_height, 3);

		if(weighted_area < 0 || weighted_area > INT_MAX)
			weighted_area = INT_MAX;

		return weighted_area;
	}

	//fully realize the shape specified by the index
	//then repeats for all contained wrappers
	//lower in the hierarchy
	void solidifyShape(int index)
	{
		//recursive base case
		if(base_level)
		{
//cout << "solidifyShape() base block!\n";
			//given the desired shape of this kernel, apply correct EP
			int size = base_block->getPossibleKernels().size();
			if(index < size)
			{
				base_block->copyDataFrom(base_block->getPossibleKernels()[index]);
				base_block->setRotation(0);
			}
			else
			{
				//cout << "CHOSEN FROM ROTATED BLOCKS!\n";
				base_block->copyDataFrom(base_block->getPossibleKernels()[2*(size-1) - index]);
				//was chosen from rotated blocks!
				base_block->setRotation(90);
			}

			base_block->computePerformance();
			setWidth(base_block->getWidth());
			setHeight(base_block->getHeight());

			return;	
		}

//cout << "\n\nSetting shape of Block_Wrapper: (" <<shapes[index].first<<", "<<shapes[index].second<<")\n\n";

		//else, it is not base_level
		setWidth(shapes[index].first);
		setHeight(shapes[index].second);

		//find the shapes which correspond to this larger block's shape
		if(cut == 'h')
		{
			for(unsigned int i = 0; i < w1->shapes.size(); i++)
				if(w1->shapes[i].first <= getWidth())
				{
					w1->solidifyShape(i);
					break;
				}

			for(unsigned int i = 0; i < w2->shapes.size(); i++)
				if(w2->shapes[i].first <= getWidth())
				{
					w2->solidifyShape(i);
					break;
				}
		}
		else //it is a 'v' cut
		{
			for(int i = w1->shapes.size()-1; i >= 0; i--)
				if(w1->shapes[i].second <= getHeight())
				{
					w1->solidifyShape(i);
					break;
				}

			for(int i = w2->shapes.size()-1; i >= 0; i--)
				if(w2->shapes[i].second <= getHeight())
				{
					w2->solidifyShape(i);
					break;
				}
		}
	} //end solidifyShape()

	//return true if the block fits in
	bool isLegal()
	{
		if(getWidth() < WSE_width && getHeight() < WSE_height)
			return true;
		else
			return false;
	}

//PRINTERS
	void printShapes()
	{
		if(!print) return;
		cout << "\nBlock_Wrapper shapes:\t"; 
		if(base_level)
			cout << "(Base Block: " << base_block->getName() << ")\n";
		else cout << "(" << cut << " cut)\n";
		for(unsigned int i = 0; i < shapes.size(); i++)
		{
			cout << "(" << shapes[i].first << ", " << shapes[i].second << ")\n";
		}
	}

}; //end Block_Wrapper

#endif


 
