#include "rectangle.h"
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



std::vector<gt_split> getgtvalues(std::string pathTogtFile){

	string STRING;
	ifstream infile;
	infile.open(pathTogtFile);

	if (infile.is_open())
	{

		vector<gt_split> vecrec;
		
		while(!infile.eof()) // To get you all the lines.
		{

			getline(infile,STRING);

			if(STRING.compare("") != 0){
				vector<string> strs;
				boost::split(strs,STRING,boost::is_any_of(";"));

				//splits gt  text
				gt_split data3;
				data3.frameNr = atoi(strs[0].c_str());

				data3.Xrectpos = atoi(strs[1].c_str());
				data3.Yrectpos = atoi(strs[2].c_str());
				data3.rectwidth = atoi(strs[3].c_str());
				data3.rectheight = atoi(strs[4].c_str());

				vecrec.push_back(data3);
			}
		}

		
		infile.close();

		return vecrec;
	}
	else
	{
		cout << "Could not open the groundtruth text file" << endl;
		exit(1);
	}
}

gt_split getRectangleAtFrame(vector<gt_split> w, int g){
	gt_split empty;
	empty.frameNr = -1;
	empty.Xrectpos = -1;
	empty.Yrectpos = -1;
	empty.rectwidth = -1;
	empty.rectheight = -1;
	
	for (int i=0; i<w.size(); i++)
	{
		if (w[i].frameNr == g)
		{
			return w[i];
		}
	}
	
	return empty;
}