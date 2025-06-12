#include "utils.h"
#include <fstream>
void writeVectorToFile(const std::vector<int>& data, const std::string& filename);

String openVid(String src_vid_path, String file_to_write_path ) {
    /**/
//    if ( argc != 2 ) {
//        std::cerr << "Usage: " << argv[0] << " <Video_Path>" << std::endl;
//        return "Usage: function <Video_Path>";
//    }
    #ifdef CAMERAXAPP_OOK_HPP
    // Define the video capture object
    VideoCapture cap(src_vid_path);
    // Create a FrameManager object
    OOK::FrameManager fm(cap);
    // Create a SignalManager object
    OOK::SignalManager sm;
    // Create a Demodulator object
    OOK::Demodulator dem(FPS, TS);
    // Create a queue
    sm.create_queue();

    // Iterate while the video has frames
    int i = 0;
    Mat frame;
    do {
        i += 1;
        // Get a frame
        frame = fm.get_frame();
        if (frame.empty()) {
            break; // Exit the loop if the frame is empty
        }
        // Detect 
        frame = fm.detector(frame, WIDTH, HEIGHT);
        // Postprocessing (Return the sum)
        double sum = fm.postprocessing(frame, BIN_THR, EROSION_SIZE);
        // Determine on or off (1 or 0)
        double a = 0;
        int bit = sm.on_or_off(sum, a, ALPHA);
        // Add to the queue
        sm.add_to_queue(bit);
    } while (true);

    // Convert the queue to a vector
    vector<int> signal = sm.queue_to_vector();
    // Demodulate the signal
    vector<int> bits = dem.demodulate(signal);

    // Synchronize the signal
    vector<int> sync = dem.synchronize(bits, OOK_HEADER, pkg_size, pkg_count, guard_size);
    // Write the vector to a file
    writeVectorToFile(sync, file_to_write_path);
    // Convert to string from ascii
    string str = dem.bits_to_ascii(dem.bits_to_string(sync));
    // Print the string 
    cout << "String: " << str << endl;
    return str;

    #endif
    #ifdef CAMERAXAPP_CSK_HPP
    cout << "Module not implemented" << endl;
    #endif
    #ifndef CAMERAXAPP_OOK_HPP
    #ifndef CAMERAXAPP_CSK_HPP
    cout << "No module was added" << endl;
    #endif
    #endif

}

void writeVectorToFile(const vector<int>& data, const string& filename) {
    // Create an output file stream
    std::ofstream outFile;
    outFile.open(filename);
    // Check if the file stream was successfully opened
    if (!outFile) {
        std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
//        LOG_INFO("Error al abrir el archivo");
        return;
    }

    // Write each element of the vector to the file
    for (const auto& value : data) {
        outFile << value << ","; // Writing each value on a new line
    }
    // Close the file stream
    outFile.close();
    std::cout << "Data successfully written to " << filename << std::endl;
//    LOG_INFO("Data successfully written");
}