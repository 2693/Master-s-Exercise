#include "trackingMode.h"

Rect RecenterWindow(Rect win, Rect now)
{
	Rect winCorr = win;

	Point nowC;
	nowC.x = now.x + now.width/2;
	nowC.y = now.y + now.height/2;

	winCorr.width = now.width*2;
	winCorr.height = now.height*2;

	winCorr.x = nowC.x - winCorr.width/2;
	winCorr.y = nowC.y - winCorr.height/2;

	return winCorr;
}

void track()
{
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
	cv::Rect win;

	//initialize tracker
	KUTracker trk;
	
	trk.SetCropImage(true);
	trk.SetSearchWin(true);
	trk.SetSearchWinLatice(10);

	TTeleData currData = getTelemetryData(line, groundHeight);
	TTeleData prevData;

	vector<double> overlapValues;
	vector<bool> inRangeValues;
	
	//create high gui output window sensor image
	namedWindow("Sensor Image");
		
	for (int j = ex.sequence.startframe; j <= ex.sequence.endframe; j++)
	{

		getline(infile,line); // Saves the line in line.
			
		prevData = currData;
		currData = getTelemetryData(line, groundHeight);
			
		std::stringstream imgfilename;
		imgfilename << ex.dir << "\\photos\\" << std::setfill('0') << std::setw(6) << Counter << ".jpg";

		SensorImage = imread(imgfilename.str());

		if(j == ex.sequence.startframe){
			trk.Init(&(IplImage)SensorImage, rect);

			//creating a search window (based on rect(t-1)).
			int cx = rect.x + (rect.width / 2);
			int cy = rect.y + (rect.height / 2);
			
			win.x = cx - rect.width;
			win.y = cy - rect.height;
			win.width = 2 * rect.width;
			win.height = 2 * rect.height;
		}

		Counter++;

		//Predict new window
		cv::Size imgSize(SensorImage.cols, SensorImage.rows);
		cv::Rect winPred = CorrectAll(win, currData, prevData, imgSize, KFOV);
		
		win = winPred;

		//Call the tracker here.
		Rect inRect = Rect(rect);
		rect = trk.Search(&(IplImage)SensorImage, inRect, win);
		
		cv::Rect winCtr = RecenterWindow(win, rect);
		win = winCtr;

		//Draw current rectangle (rect(t)).
		cv::rectangle(SensorImage, rect, cv::Scalar(0,255,0,0), 2);

		//Draw windows: predicted and actual.
		cv::rectangle(SensorImage, win, cv::Scalar(0,0,255,0), 1);
		cv::rectangle(SensorImage, winPred, cv::Scalar(0,0,128,0), 1);
		
		//Draw GT
		pair<int, cv::Rect> ROIpair = getRectangleAtFrame(ex.gt_rois,j);
		cv::rectangle(SensorImage, ROIpair.second, cv::Scalar(0,192,0,0), 1);
		
		//Show Source Image in highgui
		imshow("Sensor Image",SensorImage);

		//Required for highgui to show images
		waitKey(10);

		//TODO: For overlap we need GT to be loaded!!
		double overlap = Overlap(rect, ROIpair.second) * 100.0;
		overlapValues.push_back(overlap);
		
		//TODO: centre to centre distance < thres? 1:0

		//cout << "\rReal Height: " << currData.realHeight << " [m]. Overlap: " << overlap << " %. Object is " << contain << " % contained.";
	
			//Assemble Filename
			ostringstream Filename;

			//Save images to a file
			//Add Counter and filetype
			Filename << ex.dir << "\\photostrackingMode\\" << std::setfill('0') << std::setw(6)  << Counter << ".jpg";

			//save Sensor Image to jpg file
			imwrite(Filename.str().c_str(), SensorImage);
	
	}
	infile.close();
		
	//Calculate mean and stddev.
	pair<double, double> meansdevOverlap = CalculateMeanStdDev(overlapValues);
	//pair<double, double> meansdevInRange = CalculateMeanStdDev(inRangeValues);

	cout << endl << endl;
	cout << " == Summary ===============================" << endl;
	cout << "  Overlap  mean/sdev: " << meansdevOverlap.first << " ~ " << meansdevOverlap.second << endl;
	//cout << "  Ctd'ness mean/sdev: " << meansdevContain.first << " ~ " << meansdevContain.second << endl;
	cout << " ==========================================" << endl << endl;
	Util::Pause();

	destroyAllWindows();
}
