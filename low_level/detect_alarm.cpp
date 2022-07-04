// Include Libraries
#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

int main()
{
	// initialize a video capture object
	cv::VideoCapture vid_capture("James.mp4");
	// Print error message if the stream is invalid
	if (!vid_capture.isOpened())
	{
		std::cout << "Error opening video stream or file" << std::endl;
	}

	else
	{
		// Obtain fps and frame count by get() method and print
		// You can replace 5 with CAP_PROP_FPS as well, they are enumerations
		int fps = vid_capture.get(5);
		std::cout << "Frames per second :" << fps;

		// Obtain frame_count using opencv built in frame count reading method
		// You can replace 7 with CAP_PROP_FRAME_COUNT as well, they are enumerations
		int frame_count = vid_capture.get(7);
		std::cout << "  Frame count :" << frame_count;
	}

	bool imageWritten = true;

	// Read the frames to the last frame
	while (vid_capture.isOpened())
	{
		// Initialise frame matrix
		cv::Mat frame;
		
	    // Initialize a boolean to check if frames are there or not
		bool isSuccess = vid_capture.read(frame);	

		cv::dnn::Net net = cv::dnn::readNetFromDarknet("yolov4.cfg", "yolov4.weights");	
		net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
		net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

		// If frames are present, show it		
		if(isSuccess == true)
		{
			//display frames
			//imshow("Frame", frame);
			if(imageWritten) 
			{
				

				cv::imwrite("./testing.jpg", frame);
				std::cout << "Here" << std::endl;
				imageWritten = false;
			}
		}

		// If frames are not there, close it
		if (isSuccess == false)
		{
			std::cout << "Video camera is disconnected" << std::endl;
			break;
		}
		
		//wait 20 ms between successive frames and break the loop if key q is pressed
		// int key = waitKey(20);
		// if (key == 'q')
		// {
		// 	cout << "q key is pressed by the user. Stopping the video" << endl;
		// 	break;
		// }


	}
	// Release the video capture object
	vid_capture.release();
	//destroyAllWindows();
	return 0;
}