#include "img_proc.hpp"

/*
IMAGE PROCESSING
Developed by: Fernanda Borja
This file contains all functions that process images, such as filters and transformations. (inplace)
*/
// Crop and resize (inplace)
void crop_img(Mat& image, const Rect& roi){
    image = image(roi);
}

void resize_img(Mat& image, const Size& size, const double& fx, const double& fy, const int& interpolation){
    resize(image, image, size, fx, fy, interpolation);
}

// Color space transformations
void convert_color(Mat& image, const int& code){
    cvtColor(image, image, code);
}

// Binarization
void binarize_img(Mat& image, const double& threshold, const double& max_val, const int& type){
    // Ensure the image is in grayscale
    if (image.channels() > 1) {
        cvtColor(image, image, COLOR_BGR2GRAY);
    }
    cv::threshold(image, image, threshold, max_val, type);
}

// Otsu's binarization
void otsu_binarize(Mat& image){
    // Ensure the image is in grayscale
    if (image.channels() > 1) {
        cvtColor(image, image, COLOR_BGR2GRAY);
    }

    // Apply Otsu's binarization
    double otsu_thresh_val = cv::threshold(image, image, 0, 255, THRESH_BINARY | THRESH_OTSU);
}

// Contrast and brightness
void adjust_contrast_brightness(Mat& image, const double& alpha, const int& beta){
    image.convertTo(image, -1, alpha, beta);
}

void erode_img(Mat& src, Mat& dst, int erosion_size) {
    // Create structuring elements for erosion and dilation
    Mat element_ero = getStructuringElement(MORPH_ELLIPSE,
                                            Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                            Point(erosion_size, erosion_size));

    // Apply erosion
    erode(src, dst, element_ero);
}

void dilate_img(Mat& src, Mat& dst, int dilation_size) {
    // Create structuring elements for erosion and dilation
    Mat element_dil = getStructuringElement(MORPH_ELLIPSE,
                                            Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                            Point(dilation_size, dilation_size));

    // Apply dilation
    dilate(src, dst, element_dil);
}
 // Masks 
 