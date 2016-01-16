#include <iostream>
#include <cstring>

#include "captureMode.h"
#include "demoMode.h"
#include "trackingMode.h"

#include "util.h"

using namespace std;

void main(int argc, char** argv)
{
	//Reads user's input
	while(true){
		string clr(25, '\n');
		char p;
		
		cout << clr;
		cout << "  Kingston University London - PROACTIVE Tracking from UAVs  " << endl;
		cout << " ----------------------------------------------------------- " << endl << endl;
		
		cout << "Please select an option:" << endl << endl;
		cout << " C)apture mode" << endl;
		cout << " D)emo mode" << endl;
		cout << " T)racking mode" << endl << endl;
		cout << " Q)uit" << endl;

		cin >> p;

		if(p == 'c'|| p == 'C')
		{
			startCapture();	
		}
		else if (p == 'd' || p == 'D')
		{
			demo();
		}
		else if(p == 't' || p == 'T')
		{
			track();
		}
		else if(p == 'q' || p == 'Q'){
			return;
		}
	}
}