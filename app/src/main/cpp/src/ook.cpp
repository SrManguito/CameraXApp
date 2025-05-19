#include "ook.hpp"

namespace OOK {
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

    double FrameManager::postprocessing(Mat& img, const double threshold, const int erosion_size) {
        // Binarize the QR code (if threshold is -1, then use Otsu)
        if (threshold == -1) {
            otsu_binarize(img);
        } else {
            binarize_img(img, threshold, 255, THRESH_BINARY);
        }
        // Erode the QR code
        erode_img(img, img, erosion_size);
        // Dilate the QR code
        // dilate_img(img, img, 10);
        // Sum the pixels
        double sum = cv::sum(img)[0];
        return sum;
    }
    // ==================== SignalManager ====================
    // Init
    SignalManager::SignalManager() {
        // Empty constructor
    }
    // Functions
    // At this point, the frames are binarized
    int SignalManager::on_or_off(double &sum, double &threshold, const double factor) {
        
        // If the sum is greater than the threshold, then it's a 1, else it's a 0
        int bit = (sum > (threshold * factor)) ? 1 : 0;
        // At the same time, if the sum is greater than the threshold, update the threshold
        threshold = (sum >= threshold) ? sum : threshold;
        return bit;
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
    vector<int> Demodulator::demodulate(const vector<int>& signal) {
        // Pre-calculate constants
        const int threshold = static_cast<int>(fps * ts);
        const double halfThreshold = threshold / 2.0;
        
        // Pre-allocate memory for the result
        const int expectedBits = signal.size() / threshold + (signal.size() % threshold > 0 ? 1 : 0);
        vector<int> bits;
        bits.reserve(expectedBits);
        
        // Process in chunks
        int sum = 0;
        
        // Main processing loop
        for (size_t i = 0; i < signal.size(); ++i) {
            sum += signal[i];
            
            // When we've accumulated a full chunk of samples
            if ((i + 1) % threshold == 0) {
                bits.push_back(sum >= halfThreshold ? 1 : 0);
                sum = 0;
            }
        }
        
        // Handle any remaining data (partial chunk at the end)
        if (signal.size() % threshold > 0 && sum > 0) {
            // Scale the threshold proportionally to the number of samples
            const int remainingSamples = signal.size() % threshold;
            const double scaledThreshold = halfThreshold * remainingSamples / threshold;
            bits.push_back(sum >= scaledThreshold ? 1 : 0);
        }
        
        return bits;
    }
    // Find the header
    int Demodulator::find_header(const vector<int>& signal, const vector<int>& header) {
        // The header is a demodulated 11111
        // vector<int> header = {1, 1, 1, 1, 1};
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
    

    vector<int> Demodulator::synchronize(vector<int>& signal,const vector<int>& header,const int pkg_size,const int pkg_count,const int guard_size) {
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
        // Decode the signal
        // This function is not implemented yet
        // You can implement it based on your requirements
        return signal;
    }

    // Given a vector of 1s and 0s, convert it to a string
    string Demodulator::bits_to_string(const vector<int>& bits) {
        // Pre-allocate memory to avoid reallocations
        string str;
        str.reserve(bits.size());
        
        // Direct character conversion instead of using to_string
        for (const int bit : bits) {
            str.push_back('0' + bit);
        }
        
        return str;
    }

    // Given a string of 1s and 0s, convert it to a vector
    vector<int> Demodulator::string_to_bits(const string& str) {
        // Pre-allocate memory to avoid reallocations
        vector<int> bits;
        bits.reserve(str.size());
        
        // Range-based for loop is cleaner and slightly more efficient
        for (const char c : str) {
            // Could add validation if needed: if (c != '0' && c != '1') throw...
            bits.push_back(c - '0');
        }
        
        return bits;
    }

    // Given a string of 1s and 0s, convert it to a string of ASCII characters
    string Demodulator::bits_to_ascii(const string& str) {
        // Pre-allocate memory - each 8 bits become 1 ASCII character
        string ascii;
        ascii.reserve(str.size() / 8 + 1);
        
        // Process each byte (8 bits)
        for (size_t i = 0; i < str.size(); i += 8) {
            // Handle the case where we don't have a full byte at the end
            if (i + 8 > str.size()) break;
            
            // Manual binary conversion is faster than stoi
            unsigned char c = 0;
            for (size_t j = 0; j < 8; j++) {
                c = (c << 1) | (str[i + j] == '1');
            }
            
            // Append the character to the result
            ascii.push_back(c);
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
