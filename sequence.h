#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;
using std::vector;

typedef struct
{
	std::string path;
	int startframe;
	int endframe;
	std::string name;

}TSequence;

std::vector<TSequence> getSequences(std::string pathToSequenceFile);

typedef struct
{
	std::string seqName;
	cv::Rect ROI;
}TBBox;

vector<TBBox> getBBoxes(string pathTobboxFile);

double last_height(string filename1);

vector<pair<int, cv::Rect>> getGTValues(std::string pathTogtFile);

pair<int, cv::Rect> getRectangleAtFrame(vector<pair<int, cv::Rect>>, int);

typedef struct
{
	int count;
	std::string timestamp;
	long double latitude;
	long double longitude;
	long double height1;
	long double height2;
	double realHeight;
	double roll;
	double pitch;
	double yaw; 

}TTeleData;

TTeleData getTelemetryData(string STRING, double groundHeight);