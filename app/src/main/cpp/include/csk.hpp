#ifndef CAMERAXAPP_CSK_HPP
#define CAMERAXAPP_CSK_HPP

#include <iostream>
#include <vector>
#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <bitset>
#include <img_proc.hpp>

using namespace std;
using namespace cv;

namespace CSK {
    // Color converter from cie1931 to RGB
    // CIE 1931 color space coordinates for the four colors used in QCSK
    const float QCSK[4][2] = {
        {0.169f, 0.007f},  // Should be blue S0 00
        {0.011f, 0.460f},  // Should be cyan S1 01
        {0.734f, 0.265f},  // Should be red S2 10
        {0.402f, 0.597f}   // Should be yellow S3 11
    };

    const float Y = 1.0f; // Luminance value for conversion

    /*
    Color conversion to writeable RGB format
    */

    float clamp(float v, float minVal = 0.0f, float maxVal = 1.0f);

    float fastGammaCorrect(float u);

    void XYZToRGB(const float xyz[3], float rgb[3]);

    int convertCoords(const float xy[2], float Y);
    // Classes
    // ==================== FrameManager ====================
    class FrameManager {
        private:
        VideoCapture cap;
        Mat frame;
        public:
        // Init
        FrameManager(VideoCapture& cap);
        // Functions
        Mat get_frame();
        void show_frame();
        void release_frame();
        Mat detect_qr();
        void draw_qr(const Mat& bbox);
        Mat qr_cut(const OutputArray& bbox);
        Mat detector(Mat& img, const int width, const int height);
        int postprocessing(Mat& qr_code,const double threshold, const int erosion_size);
    };

    // ==================== SignalManager ====================
    class SignalManager {
        private:
        Mat frame;
        queue<int> signal; 
        public:
        // Init
        SignalManager();
        // Functions
        int closest_color(const vector<vector<int>>& colors, const vector<int>& color);
        void create_queue();
        void add_to_queue(int bit);
        vector<int> queue_to_vector();
        };

    // ==================== Demodulator ====================
    class Demodulator {
        private:
        vector<int> signal;
        int fps;
        float ts;
        public:
        // Init
        Demodulator(int fps, float ts);
        // Functions
        int find_header(vector<int> signal, vector<int> header);
        vector<int> synchronize(vector<int> signal, vector<int> header, int pkg_size, int pkg_count, int guard_size);
        vector<int> demodulate(vector<int> signal);
        vector<int> decode(vector<int> signal);
        string bits_to_string(vector<int> bits);
        vector<int> string_to_bits(string str);
        string bits_to_ascii(string str);
        string ascii_to_bits(string ascii);
    };
    // Functions 
}

#endif