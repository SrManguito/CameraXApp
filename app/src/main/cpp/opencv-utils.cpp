//
// Created by TheSy on 15-10-2024.
//

#include "opencv-utils.h"
//#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <android/log.h>
#define LOG_TAG "CPP_LOG"
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


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

double detect( Mat src_gray, double threshold_value){

    int rows = src_gray.rows;          // Number of rows (height)
    int cols = src_gray.cols;          // Number of columns (width)

    int width = 60, height = 60;
    int startX = cols/2;
    int startY = rows/2;

    Mat cropped_image = src_gray(Range(startY - height,startY + height), Range(startX - width, startX + width));
    Mat dst, erosion_dst; // dilation_dst;

    threshold( cropped_image, dst, threshold_value, 255, THRESH_BINARY );
    int erosion_size = 3;
//    int dilation_size = 3;

    Mat element_ero = getStructuringElement( MORPH_ELLIPSE,
                                             Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                             Point( erosion_size, erosion_size ) );

//    Mat element_dil = getStructuringElement( MORPH_ELLIPSE,
//                                             Size( 2*dilation_size + 1, 2*dilation_size+1 ),
//                                             Point( dilation_size, dilation_size ) );

    cv::erode(dst, erosion_dst, element_ero);
    double sum = cv::sum(erosion_dst)[0];

    return sum;
}

vector<int> fWindow(VideoCapture vid, int win_size, double th1 , double trust_fact ){

    double max_sum = 0;

    vector<int> decoded;
    Mat frame;

    bool broken = true;

    while (broken){

        int current_sum = 0;

        for (int j = 0; j < win_size; j++){

            vid >> frame;
            if (frame.empty()){
                broken = false;
                break;
            }
            cvtColor(frame,frame, COLOR_RGB2GRAY);
            double detection = detect(frame, th1);

            if(detection > max_sum ){
                max_sum = detection;
            }

            if(detection > max_sum * trust_fact){
                current_sum += 1;
            }
            else{
                current_sum += 0;
            }
        }


        if(current_sum >1){
            decoded.push_back(1);
        }
        else{
            decoded.push_back(0);
        }


    }

    return decoded;
}

/**
 * @brief Detects a package based on the package format of Matus paper
 */
char matusByteCharDemodulation(vector<int> package, int p_begin){
    int num = 0;
    vector<int> pos = {1,2,3,4,6,7,8,9};
    vector<int> square = {7,6,5,4,3,2,1,0};

    for(int i = 0; i<8; i++){
        cout << package[p_begin + pos[i]] << " ";
        num += package[p_begin + pos[i]] * pow(2,square[i]);
    }

    return static_cast<char>(num);
};


/**
 * @brief Detects the headers containted in the data vector
 *
 * @param
 * @param
 * @param
 *
 * @return Returns the positions of the headers
 */
String detectAndDemodulate(vector<int> data, int header_len, int package_len){

    String message;

    int data_len = data.size();

    for(int i = 0; i < data_len - header_len + 1; i++){
        int win_sum = 0;

        for(int j = 0; j < header_len; j++){
            win_sum += data[ i+j];
        }
        // std :: cout << win_sum << " ";
        if(win_sum == 5){
            cout << "Header found" << endl;

            message += matusByteCharDemodulation(data, (i + header_len));
            i += 9;
        }
        win_sum = 0;
    }
    return message;

};

void writeVectorToFile(const std::vector<int>& data, const std::string& filename) {
    // Create an output file stream
    std::ofstream outFile;
    outFile.open(filename);
    // Check if the file stream was successfully opened
    if (!outFile) {
        std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
        LOG_INFO("Error al abrir el archivo");
        return;
    }

    // Write each element of the vector to the file
    for (const auto& value : data) {
        outFile << value << ","; // Writing each value on a new line
    }
    outFile << "\n";
    // Close the file stream
    outFile.close();
    std::cout << "Data successfully written to " << filename << std::endl;
    LOG_INFO("Data successfully written");
}

String openVid(String src, String out){
    VideoCapture capture( src );
    if (!capture.isOpened()){
        //error in opening the video input
        std:: cerr << "Unable to open: " << src << std:: endl;
        return "Error abriendo el archivo";
    }
//    double vid_len = capture.get(CAP_PROP_FRAME_COUNT);
//    double count = 0;
//    Mat frame;

    vector<int> demodulated;
    demodulated = fWindow(capture, 3, 220, 0.6);
//    std::string filename = "/storage/emulated/0/Android/data/com.example.cameraxapp/files/output.txt";
//    std::string filename = "/data/data/com.example.cameraxapp/files/output.txt";


    // Write the vector to the file
    writeVectorToFile(demodulated, out);

    int header_len = 5;
    int package_len = 8;

    String decoded;
    decoded = detectAndDemodulate(demodulated, header_len, package_len);

//    while (true){
//        capture >> frame;
//        if (frame.empty()){
//            break;
//        }
//        count++;
//    }

    String demo = "Demodulated";
    return decoded;

}


