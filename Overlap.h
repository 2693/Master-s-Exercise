#pragma once

#include "opencv2/opencv.hpp"
#include <math.h>

using namespace std;

double Containedness(cv::Rect bbox, cv::Rect swindow);

double Overlap(cv::Rect result, cv::Rect expected);

pair<double,double> CalculateMeanStdDev(vector<double>);