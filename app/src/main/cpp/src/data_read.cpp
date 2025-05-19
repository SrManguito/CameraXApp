#include "data_read.hpp"

/*
DATA_READ
Developed by: Fernanda Borja
This file contains all functions that read and return data, wich are used in the main.cpp file.

* read_image: reads an image from a file and returns it as a cv::Mat object.
* read_video: reads a video from a file and returns it as a cv::VideoCapture object.
* take_picture: takes a picture from a camera and returns it as a cv::Mat object.
* take_video: takes a video from a camera and returns it as a cv::VideoCapture object.
* Steaming function is declared in main file.
*/

Mat read_image(const string& path, ImreadModes format){
    Mat image = imread(path, format);
    if (!image.data)
    {
        cerr << "No image data: " << path << "\n";
    }
    return image;
}

VideoCapture read_video(const string& path){
    VideoCapture video;
    video.open(path);
    if (!video.isOpened())
    {
        cerr << "Could not open the video file: " << path << "\n";
    }
    return video;
}

Mat take_picture(VideoCapture& camera){
    Mat frame;
    // Ensure the camera is open
    if (!camera.isOpened()) {
        cerr << "Error: Could not open camera!" << endl;
        return Mat();
    }

    // Capture a frame from the camera
    camera >> frame;

    if (frame.empty()) {
        cerr << "Error: Empty frame captured!" << endl;
        return Mat();
    }

    cout << "Photo captured!" << endl;
    return frame;
}

VideoCapture record_video(VideoCapture& camera, const string& output_path, int duration_seconds, int fps) {
    // Ensure the camera is open
    if (!camera.isOpened()) {
        cerr << "Error: Could not open camera!" << endl;
        return VideoCapture();
    }

    // Get the width and height of frames from the camera
    int frame_width = static_cast<int>(camera.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(camera.get(CAP_PROP_FRAME_HEIGHT));

    // Define the codec and create VideoWriter object
    VideoWriter video_writer(output_path, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frame_width, frame_height));

    if (!video_writer.isOpened()) {
        cerr << "Error: Could not open video writer!" << endl;
        return VideoCapture();
    }

    Mat frame;
    int total_frames = duration_seconds * fps;
    for (int i = 0; i < total_frames; ++i) {
        camera >> frame;
        if (frame.empty()) {
            cerr << "Error: Empty frame captured!" << endl;
            break;
        }
        video_writer.write(frame);
    }

    cout << "Video recorded successfully!" << endl;

    // Return a VideoCapture object to read the recorded video
    VideoCapture recorded_video(output_path);
    if (!recorded_video.isOpened()) {
        cerr << "Error: Could not open the recorded video file: " << output_path << "\n";
    }
    return recorded_video;
}
