//
// Created by TheSy on 18-05-2025.
//
#pragma once

#ifndef CAMERAXAPP_UTILS_H
#define CAMERAXAPP_UTILS_H

#include "include/ook.hpp"

// Define constants
const int FPS = 30;
const int WIDTH = 60, HEIGHT = 60;
const float TS = 0.1, ALPHA = 0.6;
const int EROSION_SIZE = 3;
const double BIN_THR = 220;
//double signal_thr = 0;
const int pkg_size = 4, pkg_count = 2, guard_size = 1;
const vector<int> CSK_HEADER = {1, 2, 3, 4, 1};
const vector<int> OOK_HEADER = {1, 1, 1, 1, 1};

String openVid(String src_vid_path, String file_to_write_path);

#endif //CAMERAXAPP_UTILS_H
