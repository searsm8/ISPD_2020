//program to check a WSE layout
//
//validates positions of kernels (conv blocks only)
//validates memory contraint
//checks for overlaps between kernels
//
#include "../util.h"
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace std;

class Conv 
{
private:
public:
	string name;
	int x, y, rot;
	int width, height;
	double memory;

	int H, W, C, K, R, S, T, U; //formal params
	int h, w, c, k; //execution params

//CONSTRUCTOR
	Conv(string nname, int nx, int ny, int nrot, int nH, int nW, int nC, int nK, int nR, int nS, int nT, int nU, int nh, int nw, int nc, int nk)
	{
		name = nname;
		x = nx;
		y = ny;
		rot = nrot;

		H = nH;
		W = nW;
		C = nC;
		K = nK;
		R = nR;
		S = nS;
		T = nT;
		U = nU;
		h = nh;
		w = nw;
		c = nc;
		k = nk;

		width = -1;
		height = -1;
		memory = -1;
	}

//PRINTERS

	void printConv()
	{
		cout << "\nConv " << name << "( ";
	       	cout << H << " ";	
	       	cout << W << " ";	
	       	cout << C << " ";	
	       	cout << K << " ";	
	       	cout << R << " ";	
	       	cout << S << " ";	
	       	cout << T << " ";	
	       	cout << U << " ";	
	       	cout << h << " ";	
	       	cout << w << " ";	
	       	cout << c << " ";	
	       	cout << k << " ";	
		cout << " )\n";
			
		cout << "(x, y, rot) = (" << x << ", " << y << ", R" << rot << ")\t";
		cout << "(width, height) = (" << width << ", " << height << ")\n";
		cout << "memory = " << memory << endl;
		cout << endl;

	}

	int computeWidth()
	{
		width = 3 * k;
		return width;
	}
	
	int computeHeight()
	{
		height = h * w * (c + 1);
		return height;
	}

	int computeMemory()
	{
		memory = (double)C/c * (double)K/k * R * S + (double)(W+S-1)/w * (double)(H+R-1)/h * (double)K/k;
		return memory;
	}

	void computePerformance()
	{
		computeWidth();
		computeHeight();
		computeMemory();
	}

};

/*
Conv* readConvData(string line)
{

	Conv* c = new Conv();
	return c;
}
*/

bool doOverlap(pair<int, int> l1, pair<int, int> r1, pair<int, int> l2, pair<int, int> r2)
{
	//If one rectangle is on the left side of the other
	if(l1.first >= r2.first || l2.first >= r1.first)
		return false;

	//If one rectangle is above other
	if(l1.second >= r2.second || l2.second >= r1.second)
		return false;

	return true;
}

int main()
{
	cout << "\nRunning: WSEcheck.cpp\n";
	//read layout file 
	//string filename = "ex_placement.paint";
	string filename = "C9.paint";
	cout << "Checking file: " << filename << endl;
	
	ifstream inFile;
	inFile.open(filename);
	if(!inFile)
	{
		cerr << "Unable to open file: " << filename << endl;
		exit(1);
	}

	string next;
	string name;
	int H, W, C, K, R, S, T, U, h, w, c, k;
	int x, y, rot;

	vector<Conv*> convs;


	//read in the whole file of conv kernels
	while(inFile >> next)
	{
		if(next.find("k") != string::npos)
		{
			name = next;
		}

		if(next.find("conv(") != string::npos)
		{
			//read conv parameters
			inFile >> H;
			inFile >> W;
			inFile >> C;
			inFile >> K;
			inFile >> R;
			inFile >> S;
			inFile >> T;
			inFile >> U;
			inFile >> h;
			inFile >> w;
			inFile >> c;
			inFile >> k;
			inFile >> next; //read in ")"
			inFile >> next; //read in ")"
			inFile >> next; //read in ")"
			inFile >> next; //read in ")"
			inFile >> next; //read in ")"
			inFile >> next; //read in ")"
			inFile >> next; //read in "place(xxx"
			x = stoi(split(next, "(")[1]);
			inFile >> y;
			inFile >> next;
			if(next.find("R0") != string::npos)
				rot = 0;
			else rot = 90;

			//create a new conv
			convs.push_back(new Conv(name, x, y, rot, H, W, C, K, R, S, T, U, h, w, c, k));
			
		}
	}
	
	inFile.close();

	//print the convs to show that it was read correctly
	for(unsigned int i = 0; i < convs.size(); i++)
	{
		convs[i]->computePerformance();
		//convs[i]->printConv();
	}
	cout << "\n\nNumber of convs: " << convs.size() << endl;

	//check for overlaps!
	cout << "\n\nChecking for overlaps...\n";
	int overlap_count = 0;
	for(unsigned int i = 0; i < convs.size(); i++)
	{
		bool overlap = false;
		Conv* a = convs[i];
		for(unsigned int j = 0; j < convs.size(); j++)
		{
			if(i == j) continue;
			overlap = false;
			Conv* b = convs[j];

			pair<int, int> l1, r1, l2, r2;

			l1.first = a->x;
			l1.second= a->y;
			if(a->rot == 0)
			{
				r1.first = a->x + a->width;
				r1.second= a->y + a->height;
				
			}
			else //rot == 90
			{
				r1.first = a->x + a->height;
				r1.second= a->y + a->width;
			}
	
			l2.first = b->x;
			l2.second= b->y;
			if(b->rot == 0)
			{
				r2.first = b->x + b->width;
				r2.second= b->y + b->height;
				
			}
			else //rot == 90
			{
				r2.first = b->x + b->height;
				r2.second= b->y + b->width;
			}
					
			if(doOverlap(l1, r1, l2, r2))
			{
				cout << "\n" << a->name << " overlaps with " << b->name << "!!!!!\n";
				overlap_count++;
			}
		}
	}
	cout << "\n###Total overlap count: " << overlap_count << endl;

	//find the max memory kernel
	int max_memory = 0;
	string max_name;
	for(unsigned int i = 0; i < convs.size(); i++)
	{
		if(convs[i]->memory > max_memory)
		{
			max_memory = convs[i]->memory;
			max_name = convs[i]->name;
		}
	}

	cout << "\nMax kernel memory is " << max_name << ": " << max_memory << endl;


	//check for boundary of 633 max width and height
	bool exceeds_max = false;

	//WSE_width = 600;
	//WSE_height= 600;

	for(unsigned int i = 0; i < convs.size(); i++)
	{
		if(convs[i]->rot == 0)
		{
			if(convs[i]->x + convs[i]->width > WSE_width)
			{
				cout << convs[i]->name << " exceeds max width of "<< WSE_width << "!!!" << endl;
				exceeds_max = true;
			}
			
			if(convs[i]->y + convs[i]->height > WSE_height)
			{
				cout << convs[i]->name << " exceeds max height of "<< WSE_height << "!!!" << endl;
				exceeds_max = true;
			}
		}
		else //rot == 90
		{
			if(convs[i]->x + convs[i]->height > WSE_width)
			{
				cout << convs[i]->name << " exceeds max width of "<< WSE_width << "!!!" << endl;
				exceeds_max = true;
			}
			
			if(convs[i]->y + convs[i]->width > WSE_height)
			{
				cout << convs[i]->name << " exceeds max height of "<< WSE_height << "!!!" << endl;
				exceeds_max = true;
			}
		}

	}

	if(exceeds_max)
		cout << "\nINVALID LAYOUT!!!\n";
	else cout << "Layout is legal!\n";

	exit(0);
}
