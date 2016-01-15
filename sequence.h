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
	std::string path;
	int startframe;
	int endframe;
	std::string input;

}sequence_split;

std::vector<sequence_split> getSequences(std::string pathToSequenceFile);