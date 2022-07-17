
/*
 g++ threading_test.cpp -o threading -pthread -std=c++17 `pkg-config --cflags --libs opencv4`
*/
#include<iostream>
#include <fstream>
#include<chrono>
#include<thread>
#include<vector>

#include <opencv2/opencv.hpp>

template<typename T>
void print(std::vector<T> const &v)
{
    for (auto i: v) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int start, int end){

    auto first = v.cbegin() + start;
    auto last = v.cbegin() + end + 1;

    std::vector<T> vec(first, last);
    return vec;
}

void parallelFunction(std::vector<int> &vec) {
    std::cout << "Thread has fired. " << std::endl;
    std::vector<int> sub_vec = slice(vec, 2, 5);
    print(sub_vec);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Thread has ended. " << std::endl;
}

int main() {

    

    cv::VideoCapture vid_capture("../test_videos/James.mp4");
    cv::namedWindow("Output");
	// Print error message if the stream is invalid
	if (!vid_capture.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
	}
    else {
		// Obtain fps and frame count by get() method and print
		// You can replace 5 with CAP_PROP_FPS as well, they are enumerations
		int fps = vid_capture.get(5);
		std::cout << "Frames per second :" << fps;

		// Obtain frame_count using opencv built in frame count reading method
		// You can replace 7 with CAP_PROP_FRAME_COUNT as well, they are enumerations
		int frame_count = vid_capture.get(7);
		std::cout << "  Frame count :" << frame_count << std::endl;
	}

    cv::Mat frame;
    int frameCount = 0;

    while(vid_capture.isOpened()) {

        // Initialize a boolean to check if frames are there or not
		bool isSuccess = vid_capture.read(frame);	
		// Read picture
		//frame = cv::imread("../test_images/strange_nina.jpeg");
		if (frame.empty())
		{
			std::cout << "!!! Failed video capture: image not found" << std::endl;
			// don't let the execution continue, else imshow() will crash.
		}

        char c;
        // If frames are present, show it		
		if(isSuccess) {
        
            std::cout << isSuccess << std::endl;   
            cv::imshow("Output", frame);

            c = (char)cv::waitKey(40);


        } else {

            std::cout << "Video camera is disconnected" << std::endl;
			break;            
        }

        // Exit if 'Esc' button is pressed
        if(c == 27) {
            break;
        }


        frameCount++;
    }

    // Release memory
    vid_capture.release();
    cv::destroyAllWindows();
    std::cout << "Staring main function." << std::endl;
    std::vector<int> v = { 1, 2, 3, 4, 2, 2, 4, 6, 5 };

    // Fire and forget thread
    std::thread(parallelFunction, std::ref(v)).detach();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Finishing main function." << std::endl;

    return 0;
}