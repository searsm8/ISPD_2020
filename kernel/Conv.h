//Conv.h
//represents a Convolutional kernel

#ifndef _Conv
#define _Conv

#include "Kernel.h"


class Conv : public Kernel
{
public:
	//Formal Parameters, immutable
	int H, W, R, S, C, K, T;

	//Executable Parameters.
	//Modifying these can lead to varied kernel performance
	int h_, w_, c_, k_;

	//constructors
	//
	
	Conv(const int& H,const int& W,const int& R,const int&S, const int& C, const int& K, const int& T )
	{
		this->H = H;
		this->W = W;
		this->R = R;
		this->S = S;
		this->C = C;
		this->K = K;
		this->T = T;
		this->h_ = 0;
		this->w_ = 0;
		this->c_ = 0;
		this->k_ = 0;
	}


	void printParams()
	{
		cout << "\nFormal Parameters:\n";
		cout << "H=" << H << ", ";
		cout << "W=" << W << ", ";
		cout << "R=" << R << ", ";
		cout << "S=" << S << ", ";
		cout << "C=" << C << ", ";
		cout << "K=" << K << ", ";
		cout << "T=" << T ;
		
		cout << "\nExecution Parameters:\n";
		cout << "h_=" << h_ << ", ";
		cout << "w_=" << w_ << ", ";
		cout << "c_=" << c_ << ", ";
		cout << "k_=" << k_ << "\n";
	}

	void printPerformance()
	{
		cout << "Height: " << getHeight() << endl;
		cout << "Width: " << getWidth() << endl;
		cout << "Time: " << getTime() << endl;
		cout << "Memory: " << getMemory() << endl;
	}

	int getHeight()
	{
		return 5;
	}

	int getWidth()
	{
		return 8;
	}

	int getTime()
	{
		return 5;
	}

	int getMemory()
	{
		return 5;
	}
};
#endif
