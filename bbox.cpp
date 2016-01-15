#include "bbox.h"
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <cstring>
#include <vector>

using namespace std;
using std::vector;

std::vector<bbox_split> getbbox(std::string pathTobboxFile){

	string STRING;
	ifstream infile;
	infile.open(pathTobboxFile);

	//check if file is open
	if (infile.is_open())
	{

		vector<bbox_split> vec;
		
		while(!infile.eof()) // To get you all the lines.
		{
			getline(infile, STRING);

			vector<string> strs;
			boost::split(strs,STRING,boost::is_any_of(";"));

			//splits bbox text
			bbox_split data;
			data.input = string(strs[0]);
			data.Xpos = atoi(strs[1].c_str());
			data.Ypos = atoi(strs[2].c_str());
			data.width = atoi(strs[3].c_str());
			data.height = atoi(strs[4].c_str());

			vec.push_back(data);



		}


		infile.close();

		return vec;

	}

	else 
		cout << "Could not open the bbox txt file" << endl;
	exit(1);
}