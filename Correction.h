#pragma once

#include <iostream>
#include <math.h>
#include <xtgmath.h>

#include <opencv2/opencv.hpp>
#include "sequence.h"
#include "CoordTransform.h"

double FovRatio(TTeleData telemetry, int imgWidth, double FOVAngleRads);

double pixelToUTM(TTeleData, TTeleData);

double distanceCalculate(int x1, int y1, int x2, int y2);
double distanceCalculate(cv::Point pt1, cv::Point pt2);

cv::Rect CorrectAll(cv::Rect winPrev, TTeleData current, TTeleData previous, cv::Size imgSize, double FOVAngleRads);

cv::Rect CorrectXY(cv::Rect rect, utm_t curr, utm_t prev, double fovRatio);

cv::Rect CorrectZ(cv::Rect rect, double prevHeight, double currHeight);

cv::Rect CorrectYaw(cv::Rect rect, double prevYawRads, double currYawRads, cv::Size imgSize);