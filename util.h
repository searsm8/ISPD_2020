//util.cpp
//author: Mark Sears
//
//provides utility functions
#ifndef _util
#define _util
//
//uncomment to disable assert()
//#define NDEBUG
#include <cassert>

#include <string>
#include <cstdio>
#include <vector>
#include <cmath>

using namespace std;

//GLOBAL VARIABLES
	bool print = true; //set false to surpress printing info

int MAX_ALLOWED_MEMORY = 10000; //for a single core on the WSE
int INT_MAX = 2147483647;
int WSE_width=633, WSE_height=633;

//CONSTANTS
	int VISUAL_UPDATE_INTERVAL = 10000; //how often the visualization gets updated

vector<string> split(const string& str, const string& delim)
{
	vector<string> tokens; //to be returned

	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if(pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos-prev);
		if(!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	}
	while(pos < str.length() && prev < str.length());
	return tokens;
}

double Variance(std::vector<double> samples)
{
     int size = samples.size();

     double variance = 0;
     double t = samples[0];
     for (int i = 1; i < size; i++)
     {
          t += samples[i];
          double diff = ((i + 1) * samples[i]) - t;
          variance += (diff * diff) / ((i + 1.0) *i);
     }

     return variance / (size - 1);
}

double StandardDeviation(std::vector<double> samples)
{
     return sqrt(Variance(samples));
}
#endif

