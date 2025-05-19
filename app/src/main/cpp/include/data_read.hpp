#ifndef CAMERAXAPP_DATA_READ_HPP
#define CAMERAXAPP_DATA_READ_HPP

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Functions
Mat read_image(const string& path, ImreadModes format = IMREAD_COLOR);
VideoCapture read_video(const string& path);
Mat take_picture(VideoCapture& camera);
VideoCapture record_video(VideoCapture& camera, const string& output_path, int duration_seconds, int fps = 30);
#endif // CAMERAXAPP_DATA_READ_HPP