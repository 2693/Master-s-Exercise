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
vector<TSequence> vec;

std::vector<TSequence> getSequences(std::string pathToSequenceFile){

	string STRING;
	ifstream infile;
	string filename(pathToSequenceFile);
	infile.open(filename);

	vector<TSequence> vec;
	
	if (infile.is_open())
	{	
		while(!infile.eof()) // To get you all the lines.
		{
			getline(infile, STRING);

			vector<string> strs;
			boost::split(strs,STRING,boost::is_any_of(";"));


			//splits sequence text
			TSequence data2;
			data2.path = string(strs[0]);
			data2.startframe = atoi(strs[1].c_str());
			data2.endframe = atoi(strs[2].c_str());
			data2.name = string(strs[3]);

			vec.push_back(data2);

		}

		infile.close();
	}
	
	return vec;
}

std::vector<TBBox> getBBoxes(std::string pathTobboxFile){

	string STRING;
	ifstream infile;
	infile.open(pathTobboxFile);

	vector<TBBox> vec;
		
	//check if file is open
	if (infile.is_open())
	{
		while(!infile.eof()) // To get you all the lines.
		{
			getline(infile, STRING);

			vector<string> strs;
			boost::split(strs,STRING,boost::is_any_of(";"));

			//splits bbox text
			TBBox data;
			
			data.seqName = string(strs[0]);

			data.ROI = cv::Rect(atoi(strs[1].c_str()), atoi(strs[2].c_str()),
				atoi(strs[3].c_str()), atoi(strs[4].c_str()));

			vec.push_back(data);
		}

		infile.close();
	}

	return vec;
}

double last_height(string filename1)
{
	double minHeight = DBL_MAX;

	string line;
	ifstream infile;

	infile.open(filename1);

	if(infile.is_open())
	{
		getline(infile, line); //Ignore first line.

		do // To get you all the lines.
		{
			getline(infile,line);

			if(line.compare("") != 0){
				vector<string> strs;
				boost::split(strs, line, boost::is_any_of(";"));

				double newval = atof(strs[4].c_str());

				if(newval < minHeight){
					minHeight = newval;
				}
			}

		}while(!infile.eof());

		infile.close();
	}

	return minHeight;
}

std::vector<pair<int, cv::Rect>> getGTValues(std::string pathTogtFile){

	string line;
	ifstream infile;
	infile.open(pathTogtFile);

	vector<pair<int, cv::Rect>> vec;
		
	if (infile.is_open())
	{

		while(!infile.eof()) // To get you all the lines.
		{

			getline(infile,line);

			if(line.compare("") != 0){
				vector<string> strs;
				boost::split(strs,line,boost::is_any_of(";"));

				//splits gt  text
				pair<int, cv::Rect> p;
				p.first = atoi(strs[0].c_str());

				p.second.x = atoi(strs[1].c_str());
				p.second.y = atoi(strs[2].c_str());
				p.second.width = atoi(strs[3].c_str());
				p.second.height = atoi(strs[4].c_str());

				vec.push_back(p);
			}
		}

		
		infile.close();
	}
	
	return vec;
}

pair<int, cv::Rect> getRectangleAtFrame(vector<pair<int, cv::Rect>> w, int g){
	pair<int, cv::Rect> empty;
	empty.first = -1;
	empty.second = cv::Rect();
	
	for (size_t i=0; i<w.size(); i++)
	{
		if (w[i].first == g)
		{
			return w[i];
		}
	}
	
	return empty;
}

TTeleData getTelemetryData(string line, double groundHeight){
	TTeleData data1;
	vector<string> strs;
	boost::split(strs, line, boost::is_any_of(";"));
	
	data1.count = atoi(strs[0].c_str());
	data1.timestamp = string(strs[1]);
	data1.latitude = atof(strs[2].c_str());
	data1.longitude = atof(strs[3].c_str());
	data1.height1 = atof(strs[4].c_str());
	data1.height2 = atof(strs[5].c_str());
	data1.roll = atof(strs[6].c_str());
	data1.pitch = atof(strs[7].c_str());
	data1.yaw = atof(strs[8].c_str());

	//real UAV height
	data1.realHeight = data1.height1 - groundHeight;

	return data1;
}