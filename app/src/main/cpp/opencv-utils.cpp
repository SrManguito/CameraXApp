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

double myDetector(Mat img, double th1, double th2){
    // Get the shape of the image
    cvtColor(img, img, COLOR_RGBA2GRAY);
    double rows = img.rows;          // Number of rows (height)
    double cols = img.cols;          // Number of columns (width)
    double channels = img.channels(); // Number of channels

    int width = 100, height = 100;
    int startX = cols/2;
    int startY = rows/2;

    Mat cropped_image = img(Range(startY - height,startY + height), Range(startX - width, startX + width));

    int erosion_size = 8;
    int dilation_size = 10;

    Mat dst, erosion_dst, dilation_dst;

    Mat element_ero = getStructuringElement( MORPH_ELLIPSE,
                                             Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                             Point( erosion_size, erosion_size ) );

    Mat element_dil = getStructuringElement( MORPH_ELLIPSE,
                                             Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                             Point( dilation_size, dilation_size ) );
    // dst.copySize(img);

    double thresh = 220;
    double maxValue = 255;

    threshold(cropped_image, dst, thresh, maxValue, THRESH_BINARY);

    erode(dst, erosion_dst, element_ero);
    dilate(erosion_dst, dilation_dst, element_dil);

    double sum = cv::sum(dilation_dst)[0];


    return sum;
}