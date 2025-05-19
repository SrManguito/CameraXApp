#ifndef CAMERAXAPP_OOK_HPP
#define CAMERAXAPP_OOK_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <bitset>
#include <img_proc.hpp>

using namespace std;
using namespace cv;

namespace OOK {
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
        double postprocessing(Mat& qr_code,const double threshold, const int erosion_size);
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
        int on_or_off(double &sum, double &threshold, const double factor);
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
        int find_header(const vector<int>& signal, const vector<int>& header);
        vector<int> synchronize(vector<int>& signal,const vector<int>& header,const int pkg_size,const int pkg_count,const int guard_size);
        vector<int> demodulate(const vector<int>& signal);
        vector<int> decode(vector<int> signal);
        string bits_to_string(const vector<int>& bits);
        vector<int> string_to_bits(const string& str);
        string bits_to_ascii(const string& str);
        string ascii_to_bits(string ascii);
    };
    // Functions 
}
#endif