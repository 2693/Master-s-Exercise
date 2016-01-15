#pragma once

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
	std::string input;
	int Xpos;
	int Ypos;
	int width;
	int height;

}bbox_split;

std::vector<bbox_split> getbbox(std::string pathTobboxFile);