//
// Created by TheSy on 15-10-2024.
//

#include "opencv-utils.h"
#include <opencv2/imgproc.hpp>

void myFlip(Mat src) {
    flip(src, src, 0);
}

void myBlur(Mat src, float sigma) {
    GaussianBlur(src, src, Size(), sigma);
}