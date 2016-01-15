#include "last_height.h"

double last_height(std::string filename1)
{
	double lastheight;

	string STRING;
	ifstream infile;


	infile.open(filename1);

	getline(infile,STRING); // Saves the line in STRING.

	while(!infile.eof()) // To get you all the lines.
	{



		vector<string> strs;
		boost::split(strs,STRING,boost::is_any_of(";"));

		lastheight = atof(strs[4].c_str());

		getline(infile,STRING); // Saves the line in STRING.
	}


	infile.close();

	return lastheight;
}