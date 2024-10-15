//
// Created by TheSy on 15-10-2024.
//

#ifndef CAMERAXAPP_OPENCV_UTILS_H
#define CAMERAXAPP_OPENCV_UTILS_H

#pragma once

#include <opencv2/core.hpp>

using namespace cv;

void myFlip(Mat src);
void myBlur(Mat src, float sigma);

#endif //CAMERAXAPP_OPENCV_UTILS_H

