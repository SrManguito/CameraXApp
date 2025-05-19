#ifndef CAMERAXAPP_DATA_PROC_HPP
#define CAMERAXAPP_DATA_PROC_HPP

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// Functions 
void crop_img(Mat& image, const Rect& roi);
void resize_img(Mat& image, const Size& size, const double& fx, const double& fy, const int& interpolation);
void convert_color(Mat& image, const int& code);
void binarize_img(Mat& image, const double& threshold, const double& max_val, const int& type);
void otsu_binarize(Mat& image);
void adjust_contrast_brightness(Mat& image, const double& alpha, const int& beta);
void erode_img(Mat& src, Mat& dst, int erosion_size);
void dilate_img(Mat& src, Mat& dst, int dilation_size);
#endif // DATA_PROC