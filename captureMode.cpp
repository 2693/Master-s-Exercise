#include "captureMode.h"

void startCapture(){
	char K;
	
	string dir = Util::CreateRecordingDir();
	if(dir.compare("") == 0) return;

	//My Sensor Decoder Object
	AnyCom::SensorDecoder_C SensorDecoder;

	//Create new opencv Mat Frame
	Mat SensorImage;

	//AnyCom Object for local communication
	AnyCom::AnyComObject  ScLocal;
	
	//Init the AnyCom Object
	char* cparam = std::getenv("ANYCOM_DEFAULT_SERVER");
	if(cparam == NULL){
		Util::Error("ANYCOM_DEFAULT_SERVER environment variable is not defined");
		return;
	}
	cout << "Connection string: " << cparam << endl;
	ScLocal.init(string(cparam), AnyCom::UdpLogging);
	
	//Message Handler
	AnyCom::StreamMessagesHandler_C handler;

	//Get Message by Id
	char* msg_id = std::getenv("ANYCOM_MESSAGE_ID");
	if(msg_id == NULL){
		Util::Error("ANYCOM_MESSAGE_ID environment variable is not defined");
		return;
	}
	AnyCom::StreamMessage_C StreamMessage = handler.getClassById(msg_id);

	//Register for receive
	ScLocal.subscribe(StreamMessage);

	//create high gui output window sensor image
	namedWindow("Sensor Image");

	//Filename Counter
	int Counter = 0;

	ofstream myfile;
	myfile.open(dir + "\\telemetry\\telemetry_data.txt");

	if(!myfile.is_open()){
		Util::Error("Could not create file '.\\telemetry\\telemetry_data.txt'");
		return;
	}

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
			cout << "UAV Position Lat: " << std::setprecision(12) << StreamMessage.Data.TelemetryData.latitude_deg << "   "
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


			myfile << Counter << ";" << to_simple_string(StreamMessage.Data.SensorData.Timestamp) << ";" << std::setprecision(12) << StreamMessage.Data.TelemetryData.latitude_deg << ";" 
				<< StreamMessage.Data.TelemetryData.longitude_deg << ";" << StreamMessage.Data.TelemetryData.altitude_m << ";"
				<< StreamMessage.Data.TelemetryData.altitude_gnd_m << ";" << StreamMessage.Data.TelemetryData.euler_phi_deg << ";"
				<< StreamMessage.Data.TelemetryData.euler_theta_deg << ";" << StreamMessage.Data.TelemetryData.euler_psi_deg << endl;

			//Add Counter and filetype
			Filename << dir << "\\photos\\" << std::setfill('0') << std::setw(6)  << Counter << ".jpg";

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
