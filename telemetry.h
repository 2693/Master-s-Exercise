#pragma once

#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.1415926
#define DEG_TO_RAD 0.0174532925


using namespace std;

typedef struct
{
	int count;
	std::string timestamp;
	long double latitude;
	long double longitude;
	long double height1;
	long double height2;
	double roll;
	double pitch;
	double yaw; 

}tele_split;

typedef struct
{
	long double a;
	long double b;
	long double c;
	long double d;
	long double e;
	long double f; 

}degtorad;

