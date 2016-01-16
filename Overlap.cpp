#include "Overlap.h"

double Containedness(cv::Rect bbox, cv::Rect swindow)
{
	double SI = MAX(0, MIN(bbox.br().x, swindow.br().x) - MAX(bbox.tl().x, swindow.tl().x))
		* MAX(0, MIN(bbox.br().y, swindow.br().y) - MAX(bbox.tl().y, swindow.tl().y));
	
	double c = SI / (double)bbox.area();

	return c;
}

double Overlap(cv::Rect result, cv::Rect expected){
	double SI = MAX(0, MIN(result.br().x, expected.br().x) - MAX(result.tl().x, expected.tl().x))
		* MAX(0, MIN(result.br().y, expected.br().y) - MAX(result.tl().y, expected.tl().y));
	
	double SU = result.area() + expected.area() - SI;

	return SI/SU;
}

pair<double,double> CalculateMeanStdDev(vector<double> vec){
	pair<double, double> meanStdDev;
	vector<double>::iterator it;
	
	double sum = 0;
	double n = (double)vec.size();

	it = vec.begin();
	while(it != vec.end()){
		sum += *it;
		++it;
	}
	
	double mean = sum / n;
	double var = 0.0, ep = 0.0;

	it = vec.begin();
	while(it != vec.end()){
		double diff = abs(*it - mean);
		ep += diff;
		var += (diff*diff);
		++it;
	}
	var =(var-ep*ep/n)/(n-1);
	double sdev=sqrt(var);

	meanStdDev.first = mean;
	meanStdDev.second = sdev;

	return meanStdDev;
}