
/*
 g++ threading_test.cpp -o threading -pthread -std=c++17 `pkg-config --cflags --libs opencv4`
*/
#include<iostream>
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

    cv::Mat frame;
    int frameCount = 0;

    cv::VideoCapture vid_capture("../test_videos/James.mp4");
	// Print error message if the stream is invalid
	if (!vid_capture.isOpened())
	{
		std::cout << "Error opening video stream or file" << std::endl;
	}

    while(vid_capture.isOpened()) {

        // Initialize a boolean to check if frames are there or not
		bool isSuccess = vid_capture.read(frame);	
		// Read picture
		//frame = cv::imread("../test_images/strange_nina.jpeg");
		if (frame.empty())
		{
			std::cout << "!!! Failed imread(): image not found" << std::endl;
			// don't let the execution continue, else imshow() will crash.
		}

        // If frames are present, show it		
		if(isSuccess == true) {
        
            cv::namedWindow("Video Output");
            cv::imshow("Video Output", frame);

        } else {

            std::cout << "Video camera is disconnected" << std::endl;
			break;            
        }

        frameCount++;
    }

    // Release memory
    vid_capture.release();

    std::cout << "Staring main function." << std::endl;
    std::vector<int> v = { 1, 2, 3, 4, 2, 2, 4, 6, 5 };

    // Fire and forget thread
    std::thread(parallelFunction, std::ref(v)).detach();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Finishing main function." << std::endl;

    return 0;
}