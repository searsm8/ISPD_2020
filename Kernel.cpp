//Kernel.cpp
//author: Mark Sears
//
//class to represent the kernel object for ISPD 2020
//
//each kernel consists of a set of
//"Formal Parameters" (FP) and 
//"Execution Parameters" (EP)
//Kernels also have a Performance function which determines
//Height, Width, Compute Time, and Memory used 
//based on the Parameters given

#include <stdio.h>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "util.cpp"

using namespace std;

class Kernel
{
	//formal paramaters are set by kernel definition.
	map<string, int> FP;

	//execution paramters can be altered to achieve better layout.
	map<string, int> EP;			

public:
	//constructor	
	Kernel(const string& param_str)
	{
		//expected format of params is:
		//formal params; execute params
		//params delimited by spaces
		vector<string> params = split(param_str, ";");
		
		vector<string> formals = split(params[0], " ");
		vector<string> executes= split(params[1], " ");

		for(int i = 0; i < formals.size(); i++)
		{
			int split_index = formals[i].find("=");
			if(split_index == -1) cout << "Invalid param_str for new Kernel! No = found!\n";
			string key = formals[i].substr(0, split_index);
			int value = stoi(formals[i].substr(split_index+1));

			FP.insert(pair<string, int>(key, value));
		}

		for(int i = 0; i < executes.size(); i++)
		{
			int split_index = executes[i].find("=");
			if(split_index == -1) cout << "Invalid param_str for new Kernel! No = found!\n";
			string key = executes[i].substr(0, split_index);
			int value = stoi(executes[i].substr(split_index+1));

			EP.insert(pair<string, int>(key, value));
		}

	}

	void printParameters()
	{	
		cout << "\nFormal Parameters:\n";
		map<string, int>::iterator itr;
		for(itr = FP.begin(); itr != FP.end(); ++itr)
			cout << itr->first << ": " << itr->second << "\n";
		cout << "\n";

		cout << "\nExecutable Parameters:\n";
		for(itr = EP.begin(); itr != EP.end(); ++itr)
			cout << itr->first << ": " << itr->second << "\n";
		cout << "\n";
	}

};


int main()
{
	Kernel my_kernel("H=1 W=2 F=5; h=2 w=3 c1=8 k1=8");
	my_kernel.printParameters();
}



