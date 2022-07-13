

#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

int main () {

    // Matrix for holding the image data
    cv::Mat frame;
    // Declaring a video player that will show the video
    cv::namedWindow("Video Player");
    // Declare object for capturing the video
    cv::VideoCapture cap(0);

    std::fstream fin("../test_images/cattian.jpeg", std::ios::binary);

    std::ostringstream ss;
    ss << fin.rdbuf();
    std::string picData = ss.str();

    if(!cap.isOpened()) {
        std::cout << "No video stream detected" << std::endl;
        std::system("pause");
        return -1;
    }

    // Main loop for processing video frames
    while(true) {
        cap >> frame;
        // Stop the loop if no video frame is detected
        if (frame.empty()) {
            std::cout << "No video frames detected, stoping stream." << std::endl;
            break;
        }

        cv::imshow("Video Player", frame);
        // Wait 25 milliseconds for frame processing
        char c = (char)cv::waitKey(25);

        // Exit if 'Esc' button is pressed
        if(c == 27) {
            break;
        }
    }

    // Release memory
    cap.release();
    return 0;
}