#pragma once
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstring>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <KUTrack.h>

#include "CoordTransform.h"
#include "sequence.h"
#include "Correction.h"
#include "Overlap.h"
#include "util.h"

using namespace std;
using namespace cv;

Rect RecenterWindow(Rect, Rect);
void track();