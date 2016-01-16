#pragma once
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>

#include <QtCore/qstring.h>
#include <QtCore/qdir.h>
#include <QtCore/qdatetime.h>

#include <opencv2/opencv.hpp>

#include "sequence.h"

using namespace std;
using namespace cv;

#define KFOV 0.7068583470578

#define UAVREC_DIR "UAV Recordings"
#define UAVEXP_DIR "UAV Experiments"

namespace Util{
	typedef struct{
		string dir;
		TBBox init_bbox;
		TSequence sequence;
		bool valid;
		std::vector<pair<int, cv::Rect>> gt_rois;
	}TExperiment;

	//Show errors and stop the program to see output
	void Error(std::string message);
	void Pause();

	//Creates a directory under home/UAV recordings for the new experiment
	string CreateRecordingDir();

	//Check empty recording dirs to delete them
	void CheckEmptyRecordingDirs();

	//Select folder
	string SelectExperimentFolder();

	TExperiment LoadExperiment(bool);
}