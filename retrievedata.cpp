#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
//OpenCV Includes (for imshow)
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//Timestamp functionality
#include <AnyCom/Common/TimeStamp/TimeStamp_C.hpp>
//Sensor Info handler
#include <AnyCom/Defines/Stream/StreamMessagesHandler_C.hpp>
//Sensor Data Decoder
#include <AnyCom/Common/SensorDecoder/SensorDecoder_C.hpp>
// Cross platform sleep
#include <AnyCom/Common/Sleep/Sleep_C.hpp>
#include <AnyCom/Common/TimeStamp/TimeStampPosixTime_C.hpp>
#include <windows.h>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <cstring>
#include "CoordTransform.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bbox.h"
#include "sequence.h"
#include "telemetry.h"
#include "last_height.h"
#include "rectangle.h"

using namespace std;
using namespace cv;
using std::vector;






void main ()
{

	int Counter, ch;
	char p;
	string STRING;
	ifstream infile;
	double rheight;
	double thetax,thetay;
	double x1,y1,x2,y2,cs,sn;
	double TopLx1,TopLy1,BottomRx1,BottomRy1;
	double TopLx,TopLy,BottomRx,BottomRy;




	//Reads user's input
	cout << "Please select between the stream mode(s) and the demo mode(d): " << endl;
	cin >> p;

	if(p == 's'|| p == 'S')
	{

		char K;


		//creating the folders
		system("mkdir c:\\output\\photos");
		system("mkdir c:\\output\\telemetry");


		//My Sensor Decoder Object
		AnyCom::SensorDecoder_C SensorDecoder;

		//Create new opencv Mat Frame
		Mat SensorImage;

		//AnyCom Object for local communication
		AnyCom::AnyComObject  ScLocal;


		//Init the AnyCom Object
		std::string param = std::getenv("ANYCOM_DEFAULT_SERVER");
		cout << "Connection string: " << param << endl;
		ScLocal.init(param, AnyCom::UdpLogging);

		//Message Handler
		AnyCom::StreamMessagesHandler_C handler;

		//Get Message by Id
		std::string msg_id = std::getenv("ANYCOM_MESSAGE_ID");
		AnyCom::StreamMessage_C StreamMessage = handler.getClassById(msg_id);

		//Register for receive
		ScLocal.subscribe(StreamMessage);

		//create high gui output window sensor image
		namedWindow("Sensor Image",CV_WINDOW_NORMAL | CV_GUI_EXPANDED);

		//Filename Counter
		int Counter = 0;

		ofstream myfile;
		myfile.open ("C:\\output\\telemetry\\telemetry_data.txt");

		myfile << "Counter" << ";" << "Timestamp" << ";" << "Latitude[deg]" << ";" << "Longitude[deg]" << ";" << "UAV Height ASL[m]" << ";"
			<< "UAV Height AGL[m]" << ";" << "Roll[deg]" << ";" << "Pitch[deg]" << ";" << "Yaw[deg]" << endl;



		//Main Loop
		while (true)
		{

			K = cv::waitKey(10);

			if (K == 'Q' || K == 'q')
			{
				break;
			}

			//Assemble Filename
			ostringstream Filename;


			//If new Stream Message received
			if (ScLocal.status(StreamMessage) == AnyCom::NewData)
			{
				//Read Stream Message
				ScLocal.read(StreamMessage);

				//Decode Sensor Data
				SensorDecoder.sensorData2OpenCv(StreamMessage.Data.SensorData, SensorImage);

				//Show Source Image in highgui
				imshow("Sensor Image",SensorImage);

				//Required for highgui to show images
				waitKey(10);

				//Increment Counter (only for saving images)
				Counter++;



				//Access and print Position, Altitude, Course and Opening Angles and Filename of saved Image
				cout << endl << endl << endl;
				cout << "Absolute Sensor Timestamp: " << to_simple_string(StreamMessage.Data.SensorData.Timestamp)  <<  endl;
				cout << "UAV Position Lat: " << StreamMessage.Data.TelemetryData.latitude_deg << "   "
					<< "Lon: " << StreamMessage.Data.TelemetryData.longitude_deg << endl;
				cout << "UAV Height ASL/AGL[m]: " << StreamMessage.Data.TelemetryData.altitude_m        << "  "
					<< StreamMessage.Data.TelemetryData.altitude_gnd_m    << endl;
				cout << "UAV Attitude Roll/Pitch/Yaw [deg]: " << StreamMessage.Data.TelemetryData.euler_phi_deg      << "  "
					<< StreamMessage.Data.TelemetryData.euler_theta_deg    << "  "
					<< StreamMessage.Data.TelemetryData.euler_psi_deg      <<  endl;
				cout << "Sensor Gimbal Roll/Pitch/Yaw [deg]: " << StreamMessage.Data.GimbalData.EulerPhi_deg    << "  "
					<< StreamMessage.Data.GimbalData.EulerTheta_deg                << "  "
					<< StreamMessage.Data.GimbalData.EulerPsi_deg                  <<  endl;
				cout << "Opening Angle Horizontal/Vertical [deg]: " << StreamMessage.Data.SensorData.HorizontalOpeningAngle << "  "
					<< StreamMessage.Data.SensorData.VerticalOpeningAngle << endl;


				myfile << Counter << ";" << to_simple_string(StreamMessage.Data.SensorData.Timestamp) << ";" << StreamMessage.Data.TelemetryData.latitude_deg << ";" 
					<< StreamMessage.Data.TelemetryData.longitude_deg << ";" << StreamMessage.Data.TelemetryData.altitude_m << ";"
					<< StreamMessage.Data.TelemetryData.altitude_gnd_m << ";" << StreamMessage.Data.TelemetryData.euler_phi_deg << ";"
					<< StreamMessage.Data.TelemetryData.euler_theta_deg << ";" << StreamMessage.Data.TelemetryData.euler_psi_deg << endl;

				//Add Counter and filetype
				Filename << "C:\\output\\photos\\" << std::setfill('0') << std::setw(6)  << Counter << ".jpg";

				//save Sensor Image to jpg file
				imwrite(Filename.str().c_str(), SensorImage);

				//Print Filename
				cout << "Image Name: " << Filename.str() << endl;


			}
			else
			{
				AnyCom::Sleep_C waitThread;
				waitThread.ussleep(10000);
			}
		}

		myfile.close();


	}

	else if (p == 'd' || p == 'D')
	{

		std::vector<sequence_split> w;
		w = getSequences("C:\\output\\sequences.txt");

		for (int i = 0; i < w.size(); i++)
		{
			cout << i << " " << w[i].input << endl;
		}

		//Reads user's input
		do{
			cout << "Please give a number from the list above: " << endl;
			cin >> ch;
		}while(ch <0 || ch>=w.size());

		
		cout << "You selected " << ch << endl;
		

		std::vector<bbox_split> vec;
		vec = getbbox("C:\\output\\bbox.txt");

		int k;
		for (k = 0; k < vec.size(); k++)
		{
			if(vec[k].input.compare(w[ch].input) == 0)
			{
				break;
			}
		}

		//HACK: This was inside your loop (was wrong) (Pau).
		if (k == vec.size())
		{
			cout << "The sequence.txt and bbox.txt you created are not matching" << endl;
			//system("PAUSE");
		}

		Counter = w[ch].startframe;

		string filename("C:\\output\\telemetry\\telemetry_data.txt");
		infile.open(filename);

		for (int i = 0; i < Counter; i++)
		{
			getline(infile,STRING);
		}


		//Create new opencv Mat Frame
		Mat SensorImage;

		//create high gui output window sensor image
		namedWindow("Sensor Image");

		double groundHeight = last_height(filename);

		//defining the gt vector
		std::vector<gt_split> rects;
		std::stringstream gtfilepath;
		gtfilepath << "C:\\output\\gt files\\gt_" << w[ch].input << ".txt"; //HACK: this was wrong.
		rects = getgtvalues(gtfilepath.str());

		for (int j = w[ch].startframe; j <= w[ch].endframe; j++)
		{



			getline(infile,STRING); // Saves the line in STRING.


			std::stringstream imgfilename;
			imgfilename << "C:\\output\\photos\\" << std::setfill('0') << std::setw(6) << Counter << ".jpg";

			SensorImage = imread(imgfilename.str());

			Counter++;

			vector<string> strs;
			boost::split(strs,STRING,boost::is_any_of(";"));

			tele_split data1;
			data1.count = atoi(strs[0].c_str());
			data1.timestamp = string(strs[1]);
			data1.latitude = atof(strs[2].c_str());
			data1.longitude = atof(strs[3].c_str());
			data1.height1 = atof(strs[4].c_str());
			data1.height2 = atof(strs[5].c_str());
			data1.roll = atof(strs[6].c_str());
			data1.pitch = atof(strs[7].c_str());
			data1.yaw = atof(strs[8].c_str());

			//creating a rectangle over the frame
			gt_split rect = getRectangleAtFrame(rects,j);

			if(rect.Xrectpos == -1){
				cout << "Error: Rectangle for frame " << j << " could not be found (corrupt file?)." << endl;
				exit(-1);
			}

			//HACK: creating a search window
			int cx = rect.Xrectpos + (rect.rectwidth / 2);
			int cy = rect.Yrectpos + (rect.rectheight / 2);

			gt_split searchWin;

			searchWin.Xrectpos = cx - rect.rectwidth;
			searchWin.Yrectpos = cy - rect.rectheight;
			searchWin.rectwidth = 2 * rect.rectwidth;
			searchWin.rectheight = 2 * rect.rectheight;

			cv::Rect r;
			r.x = rect.Xrectpos;
			r.y = rect.Yrectpos;
			r.width = rect.rectwidth;
			r.height = rect.rectheight;
			
			cout << j << " : " << r << endl;

			cv::rectangle(SensorImage, r, cv::Scalar(0,255,0,0), 2);

			//Show Source Image in highgui
			imshow("Sensor Image",SensorImage);

			//Required for highgui to show images
			waitKey(10);

			

			//Degrees to radians
			degtorad data2;
			data2.a = data1.roll * DEG_TO_RAD;
			data2.b = data1.pitch * DEG_TO_RAD;
			data2.c = data1.yaw * DEG_TO_RAD;


			//Long-Lat to UTM
			latlon_t UAV;
			UAV.lat = data1.latitude;
			UAV.lon = data1.longitude;
			utm_t UAVUTM;

			LLtoUTM(UAV, &UAVUTM);

			//real UAV height
			rheight = data1.height1 - groundHeight;

			//calculating the fovw/2 and fovh/2
			thetax = rheight * tan (0.7068583470578);
			thetay = rheight * tan (0.530143760293277609);

			double tlx, tly;
			double brx, bry;

			tlx = -thetax;
			tly = thetay;

			brx = thetax;
			bry = -thetay;

			//2D Euclidean vector rotations
			cs = cos(data2.c);
			sn = sin(data2.c);

			x1 = (tlx * cs) - (tly * sn);
			y1 = (tlx * sn) + (tly * cs);

			x2 = (brx * cs) - (bry * sn);
			y2 = (brx * sn) + (bry * cs);

			//Define the two corners of the rectangle

			TopLx = UAVUTM.east + x1;
			TopLy = UAVUTM.north + y1;
			BottomRx = UAVUTM.east + x2;
			BottomRy = UAVUTM.north + y2;

			//Access and print Position, Altitude, Course and Opening Angles and Filename of saved Image
			/*cout << endl << endl << endl;
			cout << "Absolute Sensor Timestamp: " << data1.timestamp  <<  endl;
			cout << "UAV Position Lat[deg] : " << data1.latitude << "   "
				<< "Lon[deg]: " << data1.longitude << endl;
			printf("%c, %i, %.8lf m (N), %.8lf m (E).\n",UAVUTM.hemisphere,UAVUTM.zoneNumber,UAVUTM.north,UAVUTM.east);
			cout << "UAV Height ASL/AGL[m]: " << rheight << endl;
			cout << "UAV Attitude Roll/Pitch/Yaw [deg]: " << data1.roll      << "  "
				<< data1.pitch << "  "
				<< data1.yaw <<  endl;
			cout << "UAV Attitude Roll/Pitch/Yaw [rad]: " << data2.a      << "  "
				<< data2.b << "  "
				<< data2.c      <<  endl;
			printf("TopLeftx: %.8lf m ,TopLefty: %.8lf m  \n",TopLx,TopLy);
			printf("BottomRigtx: %.8lf m ,BottomRigty: %.8lf m  \n",BottomRx,BottomRy);
			printf("Thetax: %.8lf m ,Thetay: %.8lf m  \n",thetax,thetay);
			*/
		}
		infile.close();
		system ("pause");
	}
}