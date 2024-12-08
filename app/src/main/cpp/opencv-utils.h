//
// Created by TheSy on 15-10-2024.
//

#ifndef CAMERAXAPP_OPENCV_UTILS_H
#define CAMERAXAPP_OPENCV_UTILS_H

#pragma once

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

void myFlip(Mat src);
void myBlur(Mat src, float sigma);
double myDetector(Mat img, double th1, double th2);
String openVid(String src, String out);

#endif //CAMERAXAPP_OPENCV_UTILS_H

