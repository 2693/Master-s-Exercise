// struct header for rectangle
#pragma once
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using std::vector;

typedef struct
{
	int frameNr;
    int Xrectpos;
	int Yrectpos;
	int rectwidth;
	int rectheight;
}gt_split;

std::vector<gt_split> getgtvalues(std::string pathTogtFile);

gt_split getRectangleAtFrame(vector<gt_split>, int);