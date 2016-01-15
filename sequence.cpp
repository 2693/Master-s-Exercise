#include "sequence.h"
#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <cstring>
#include <vector>

using namespace std;
vector<sequence_split> vec;

std::vector<sequence_split> getSequences(std::string pathToSequenceFile){

	string STRING;
	ifstream infile;
	string filename(pathToSequenceFile);
	infile.open(filename);

	if (infile.is_open())
	{
		vector<sequence_split> vec;
			
		while(!infile.eof()) // To get you all the lines.
		{
			getline(infile, STRING);

			vector<string> strs;
			boost::split(strs,STRING,boost::is_any_of(";"));


			//splits sequence text
			sequence_split data2;
			data2.path = string(strs[0]);
			data2.startframe = atoi(strs[1].c_str());
			data2.endframe = atoi(strs[2].c_str());
			data2.input = string(strs[3]);

			vec.push_back(data2);

		}


		infile.close();

		return vec;
	}
	else
		cout << "Could not open the bbox text file" << endl;
	exit(1);
}