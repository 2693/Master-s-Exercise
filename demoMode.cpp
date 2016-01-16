#include "demoMode.h"

void demo(){
	
	//Interactive function to load a sequence in an experiment
	Util::TExperiment ex = Util::LoadExperiment(true); //true means load GT as well.
	if(!ex.valid) return;

	//Frame counter (for loop)
	int Counter = ex.sequence.startframe;

	//Open telemetry data file
	string line;
	ifstream infile;

	string filename(ex.dir + "\\telemetry\\telemetry_data.txt");
	infile.open(filename);

	//Skip lines of telemetry, up to interesting point.
	for (int i = 0; i < Counter; i++)
	{
		getline(infile, line);
	}

	//Parse the file to obtain the ground height.
	cout << "Parsing height data ... ";
	double groundHeight = last_height(filename);
	if(groundHeight == DBL_MAX){
		cout << "[Error]" << endl;
		Util::Error("Could not parse telemetry data file '"+filename+"'");
		return;
	}
	cout << "[Done]" << endl;
	cout << "  Ground Height is: " << groundHeight << " [m]" << endl;

	//Create new opencv Mat Frame
	Mat SensorImage;

	//Obtain the initial seed ROI
	cv::Rect rect = ex.init_bbox.ROI;

	TTeleData currData = getTelemetryData(line, groundHeight);
	TTeleData prevData;

	vector<double> overlapValues;
	vector<double> containValues;

	//create high gui output window sensor image
	namedWindow("Sensor Image");

	for (int j = ex.sequence.startframe; j <= ex.sequence.endframe; j++)
	{

		//Assemble Filename
		ostringstream Filename;

		getline(infile,line); // Saves the line in line.
			
		prevData = currData;
		currData = getTelemetryData(line, groundHeight);
			
		std::stringstream imgfilename;
		imgfilename << ex.dir << "\\photos\\" << std::setfill('0') << std::setw(6) << Counter << ".jpg";

		SensorImage = imread(imgfilename.str());

		Counter++;

		//creating a rectangle over the frame
		pair<int, cv::Rect> ROIpair = getRectangleAtFrame(ex.gt_rois,j);

		if(ROIpair.first == -1){
			stringstream msg;
			msg << "Rectangle for frame " << j << " could not be found (corrupt file?)" << endl;
			
			Util::Error(msg.str());
			return;
		}

		//Draw previous rectangle (rect(t-1)).
		cv::rectangle(SensorImage, rect, cv::Scalar(0,128,0,0), 1);

		//creating a search window (for rect(t-1)).
		int cx = rect.x + (rect.width / 2);
		int cy = rect.y + (rect.height / 2);
			
		cv::Rect win;
		win.x = cx - rect.width;
		win.y = cy - rect.height;
		win.width = 2 * rect.width;
		win.height = 2 * rect.height;

		//Update rectangle (rect(t)).
		rect = ROIpair.second;

		//Get actual new window.
		cv::Rect winNew;
		winNew.x = cx - rect.width;
		winNew.y = cy - rect.height;
		winNew.width = 2 * rect.width;
		winNew.height = 2 * rect.height;

		//Predict new window
		cv::Size imgSize(SensorImage.cols, SensorImage.rows);
		cv::Rect winPred = CorrectAll(win, currData, prevData, imgSize, KFOV);

		//Draw previous rectangle (rect(t-1)).
		cv::rectangle(SensorImage, rect, cv::Scalar(0,255,0,0), 2);

		//Draw windows: predicted and actual.
		cv::rectangle(SensorImage, winNew, cv::Scalar(0,0,128,0), 2);
		cv::rectangle(SensorImage, winPred, cv::Scalar(0,0,255,0), 1);

		//Show Source Image in highgui
		imshow("Sensor Image",SensorImage);

		//Required for highgui to show images
		waitKey(40);
		
		double overlap = Overlap(winPred, winNew) * 100.0;
		double contain = Containedness(rect, winPred) * 100.0;

		overlapValues.push_back(overlap);
		containValues.push_back(contain);

			//Save images to a file
			//Add Counter and filetype
			Filename << ex.dir << "\\photosdemoMode\\" << std::setfill('0') << std::setw(6)  << Counter << ".jpg";

			//save Sensor Image to jpg file
			imwrite(Filename.str().c_str(), SensorImage);

		cout << "\rReal Height: " << currData.realHeight << " [m]. Overlap: " << overlap << " %. Object is " << contain << " % contained.";
	}
	infile.close();
		
	//TODO: Calculate mean and stddev.
	pair<double, double> meansdevOverlap = CalculateMeanStdDev(overlapValues);
	pair<double, double> meansdevContain = CalculateMeanStdDev(containValues);

	cout << endl << endl;
	cout << " == Summary ===============================" << endl;
	cout << "  Overlap  mean/sdev: " << meansdevOverlap.first << " ~ " << meansdevOverlap.second << endl;
	cout << "  Ctd'ness mean/sdev: " << meansdevContain.first << " ~ " << meansdevContain.second << endl;
	cout << " ==========================================" << endl << endl;
	Util::Pause();

	destroyAllWindows();
}
