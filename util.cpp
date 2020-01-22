//util.cpp
//author: Mark Sears
//
//provides utility functions

#include <string>
#include <vector>

using namespace std;

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
