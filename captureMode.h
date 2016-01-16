#pragma once
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//OpenCV Includes (for imshow)
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//Timestamp functionality
#include <AnyCom/Common/TimeStamp/TimeStamp_C.hpp>
//Sensor Info handler
#include <AnyCom/Defines/Stream/StreamMessagesHandler_C.hpp>
//Sensor Data Decoder
#include <AnyCom/Common/SensorDecoder/SensorDecoder_C.hpp>
// Cross platform sleep
#include <AnyCom/Common/Sleep/Sleep_C.hpp>
#include <AnyCom/Common/TimeStamp/TimeStampPosixTime_C.hpp>

#include "util.h"

using namespace std;
using namespace cv;

void startCapture();
