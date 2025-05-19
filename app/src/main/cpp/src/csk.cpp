#include "csk.hpp"

namespace CSK {
    // Color converter from cie1931 to RGB
    /*
    Color conversion to writeable RGB format
    */

    float clamp(float v, float minVal, float maxVal) {
        return std::max(minVal, std::min(maxVal, v));
    }

    float fastGammaCorrect(float u) {
        if (u <= 0.0031308f) {
            return 12.92f * u;
        } else {
            return 1.055f * powf(u, 1.0f/2.4f) - 0.055f;
        }
    }

    void XYZToRGB(const float xyz[3], float rgb[3]) {
        // Using sRGB conversion matrix (IEC 61966-2-1)
        rgb[0] =  3.2406f * xyz[0] - 1.5372f * xyz[1] - 0.4986f * xyz[2];
        rgb[1] = -0.9689f * xyz[0] + 1.8758f * xyz[1] + 0.0415f * xyz[2];
        rgb[2] =  0.0557f * xyz[0] - 0.2040f * xyz[1] + 1.0570f * xyz[2];

        // Clamp before gamma correction
        rgb[0] = clamp(rgb[0]);
        rgb[1] = clamp(rgb[1]);
        rgb[2] = clamp(rgb[2]);

        // Gamma correction
        rgb[0] = fastGammaCorrect(rgb[0]);
        rgb[1] = fastGammaCorrect(rgb[1]);
        rgb[2] = fastGammaCorrect(rgb[2]);
    }

    int convertCoords(const float xy[2], float Y) {
        int RGB[3] = {0, 0, 0};
        float x = xy[0];
        float y = xy[1];

        if (y <= 0.001f) { 
            // Invalid data, return black
            return 0;
        }

        // Calculate XYZ from xyY
        float X = (x / y) * Y;
        float Z = ((1.0f - x - y) / y) * Y;

        float xyz[3] = {X, Y, Z};
        float rgb[3];

        XYZToRGB(xyz, rgb);

        // Final clamp and quantization
        RGB[0] = int(clamp(rgb[0]) * 255.0f + 0.5f);
        RGB[1] = int(clamp(rgb[1]) * 255.0f + 0.5f);
        RGB[2] = int(clamp(rgb[2]) * 255.0f + 0.5f);

        // Pack RGB into a single integer (0xRRGGBB format)
        return (RGB[0] << 16) | (RGB[1] << 8) | RGB[2];
    }
    // ==================== FrameManager ====================
    // Init
    FrameManager::FrameManager(VideoCapture& capturer) : cap(capturer) {
        if (!cap.isOpened()) {
        throw std::runtime_error("Could not open video");
        }
    }
    // Functions
    Mat FrameManager::get_frame() {
        cap >> frame; // Will it work?
        // If the frame is empty, then the video has ended
        return frame;
    }

    void FrameManager::show_frame() {
        imshow("Frame", frame);
        waitKey(0);
    }

    void FrameManager::release_frame() {
        frame.release();
    }

    // Detect QR square
    Mat FrameManager::detect_qr() {
        // Create a QRCodeDetector object
        QRCodeDetector qrDecoder;
        Mat bbox;
        // Detect and give coordinates of the QR code location
        bool qr_found;
        qr_found = qrDecoder.detect(frame, bbox);
        if (qr_found) {
            return bbox;
        }
        else {
            return Mat();
        }
    }

    // Draw QR square and show it
    void FrameManager::draw_qr(const Mat& bbox) {
    if (bbox.rows == 1 && bbox.cols == 4) {
        // Extract the 4 points stored as nested Vec2f (2D float points)
        std::array<Point, 4> points;
        for (int i = 0; i < 4; ++i) {
            float x = bbox.at<Vec2f>(0, i)[0]; // x-coordinate
            float y = bbox.at<Vec2f>(0, i)[1]; // y-coordinate
            points[i] = Point(x, y);
        }

        // Draw the bounding box
        for (int i = 0; i < 4; ++i) {
            line(frame, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 2); // Green lines
        }
    } else {
        throw std::runtime_error("Unexpected bbox format");
    }
    // Show the frame with the QR code bounding box
    imshow("Frame", frame);
    waitKey(0);
}
    // Cut and binarize the QR code

    Mat FrameManager::qr_cut(const OutputArray& bbox) {
        // Get the bounding box
        Rect bbox_rect = boundingRect(bbox);
        // I know that my desired size is three times smaller than the size of the bounding box (centered)
        int new_width = bbox_rect.width / 3;
        int new_height = bbox_rect.height / 3;
        int x = bbox_rect.x + bbox_rect.width / 3;
        int y = bbox_rect.y + bbox_rect.height / 3;
        bbox_rect = Rect(x, y, new_width, new_height);
        // Crop the QR code
        Mat qr_code = frame(bbox_rect);
        return qr_code;
    }

    // Wrapper for all detection steps
    Mat FrameManager::detector(Mat& img, const int width, const int height) {
        // AMMI CASE, force the coords
        // Ensure the image is in grayscale
        if (img.channels() > 1) {
            cvtColor(img, img, COLOR_BGR2GRAY);
        }
        int rows = img.rows, cols = img.cols;
        // Define where to cut
        int x_start = (cols) >> 1;
        int y_start = (rows) >> 1;

        // Cut the image
        Mat cropped_img = img(Range(y_start - height, y_start + height), Range(x_start - width, x_start + width));
        return cropped_img;
    }

    int FrameManager::postprocessing(Mat& img, const double threshold, const int erosion_size) {
        // Resize the image
        resize(img, img, Size(), 0.5, 0.5, INTER_LINEAR);

        // Get the most common color in the image (average)
        Scalar mean_color = mean(img);
        // Pack RGB in a single integer
        int mean_color_int = (static_cast<int>(mean_color[0]) << 16) | (static_cast<int>(mean_color[1]) << 8) | static_cast<int>(mean_color[2]);
        
        return mean_color_int;
    }
    // ==================== SignalManager ====================
    // Init
    SignalManager::SignalManager() {
        // Empty constructor
    }
    // Functions
    // At this point, the frames are binarized
    int SignalManager::closest_color(const vector<vector<int>>& colors, const vector<int>& color) {
        double minDistance = numeric_limits<double>::max();
        int closestIndex = -1;

        for (size_t i = 0; i < colors.size(); ++i) {
            double distance = 0;
            for (int j = 0; j < 3; ++j) {
                distance += pow(colors[i][j] - color[j], 2);
            }
            distance = sqrt(distance);

            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = static_cast<int>(i);
            }
        }

        return closestIndex;
    }

    // Create a queue
    void SignalManager::create_queue() {
        signal = queue<int>();
    }

    // Add to queue
    void SignalManager::add_to_queue(int bit) {
        signal.push(bit);
    }
    // queue to vector
    vector<int> SignalManager::queue_to_vector() {
        vector<int> vec;
        while (!signal.empty()) {
            vec.push_back(signal.front());
            signal.pop();
        }
        return vec;
    }

    // ==================== Demodulator ====================
    // Init
    // fps = frames per second, ts = simbol period in seconds
    Demodulator::Demodulator(int f, float t) : fps(f), ts(t) {
    }
    // Functions
    // Given the raw detected signal, convert it to bits
    vector<int> Demodulator::demodulate(vector<int> signal) {
        // Pre-calculate constants
        const int threshold = static_cast<int>(fps * ts);
        // Pre-allocate memory for the result
        const int expectedBits = signal.size() / threshold + (signal.size() % threshold > 0 ? 1 : 0);
        vector<int> bits;
        bits.reserve(expectedBits);
        // Process in chunks
        int sum = 0;
        // Main processing loop
        for (size_t i = 0; i < signal.size(); ++i) {
            sum += signal[i];
            // Get the average color seen so far
            if (i % threshold == 0) {
                int avg = sum / threshold;
                // This number should be between 0 and 3
                bits.push_back(avg);
                // Reset the sum
                sum = 0;
            }
        }

        // Handle the case where we have a partial chunk at the end
        if (signal.size() % threshold > 0) {
            // Scale the threshold proportionally to the number of samples
            const int remainingSamples = signal.size() % threshold;
            int avg = (sum) / remainingSamples;
            bits.push_back(avg);
        }
        // Return the bits
        return bits;
    }

    // Find the header
    int Demodulator::find_header(vector<int> signal, vector<int> header) {
        // The header is a demodulated 11111
        // vector<int> header = {1, 2, 3, 4, 1};
        int h_size = header.size();
        int found = -1;
        for (int i = 0; i < signal.size() - h_size; i++) { // Ensure bounds are respected
            if (signal[i] == header[0]) {
                for (int j = 0; j < h_size; j++) {
                    if (signal[i + j] != header[j]) {
                        break;
                    }
                }
                    found = i + h_size; // Return the index after the header
                    break;
                }
            }
        return found; // Explicitly return false if the header is not found
        }
    

    vector<int> Demodulator::synchronize(vector<int> signal, vector<int> header, int pkg_size, int pkg_count, int guard_size) {
        // Start with an empty signal
        vector<int> bits;
        int index;
        do {
            index = find_header(signal, header);
            // If the header is not found, then break
            if (index == -1) {
                break;
            }
            // Cut if too short
            int required_size = (pkg_size * pkg_count) + index + (guard_size * (pkg_count + 1));
            if (signal.size() < required_size) {
                break;
            }
            // Add the following pkg_size bits to the signal
            // A package is gbbbbgbbbbg, we add only the bbbb's
            int idx = 0;
            for (int idx = 0; idx < pkg_count; ++idx) {
                int start = (1 + idx) + (pkg_size * idx) + index;
                bits.insert(bits.end(), signal.begin() + start, signal.begin() + start + pkg_size);
            }
            // Remove the bits from the signal and header
            signal.erase(signal.begin(), signal.begin() + required_size);
        } while (true);

        return bits;
    }
    

    vector<int> Demodulator::decode(vector<int> signal) {
        return signal;
    }

    // Given a vector of 1s and 0s, convert it to a string
    string Demodulator::bits_to_string(vector<int> symbols) {
    string str;
    for (int sym : symbols) {
        // Convert each symbol (0-3) to its 2-bit binary representation
        str += bitset<2>(sym).to_string();
    }
    return str;
}

    // Given a string of 1s and 0s, convert it to a vector
    vector<int> Demodulator::string_to_bits(string str) {
        vector<int> bits;
        for (int i = 0; i < str.size(); i++) {
            bits.push_back(str[i] - '0');
        }
        return bits;
    }

    // Given a string of 1s and 0s, convert it to a string of ASCII characters
    string Demodulator::bits_to_ascii(string str) {
        string ascii;
        for (int i = 0; i < str.size(); i += 8) {
            string byte = str.substr(i, 8);
            char c = stoi(byte, nullptr, 2);
            ascii += c;
        }
        return ascii;
    }

    // Given a string of ASCII characters, convert it to a string of 1s and 0s
    string Demodulator::ascii_to_bits(string ascii) {
        string bits;
        for (int i = 0; i < ascii.size(); i++) {
            bits += bitset<8>(ascii[i]).to_string();
        }
        return bits;
    }
    // ==================== Extra Functions ====================

}
