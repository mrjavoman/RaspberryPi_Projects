/*
g++ detect_alarm.cpp -o detect `pkg-config --cflags --libs opencv4`
*/

// Include Libraries
#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

constexpr float CONFIDENCE_THRESHOLD = 0;
constexpr float NMS_THRESHOLD = 0.4;
constexpr int NUM_CLASSES = 80;

// colors for bounding boxes
const cv::Scalar colors[] = {
    {0, 255, 255},
    {255, 255, 0},
    {0, 255, 0},
    {255, 0, 0}
};
const auto NUM_COLORS = sizeof(colors)/sizeof(colors[0]);

int main()
{
	std::vector<std::string> class_names;
    {
        std::ifstream class_file("../classes.txt");
        if (!class_file)
        {
            std::cerr << "failed to open classes.txt\n";
            return 0;
        }

        std::string line;
        while (std::getline(class_file, line))
            class_names.push_back(line);
    }

	// Generate video algorithm
	// Save frames in array as they are being processed
	// When a detection occurs save the picture of the detection
	// Get the frames from the array and through a loop put them on a video writter
	// Sample:
	// - outputFile = "yolo_out_cpp.avi";
	// - video.open(outputFile, VideoWriter::fourcc('M','J','P','G'), 28, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));
	// Then set each frame from the array into the video writter
	// - video.write(detectedFrame);
	// Finally send the video that was written through an message with the image as the thumbnail

	// initialize a video capture object
	cv::VideoCapture vid_capture("../test_videos/James.mp4");
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
		std::cout << "  Frame count :" << frame_count << std::endl;
	}

	// Initialise frame matrix
	cv::Mat frame;
	cv::Mat blob;
	std::vector<cv::Mat> detections;
	int frameCount = 0;
	bool imageWritten = true;

	// Initialize model.
	cv::dnn::Net net = cv::dnn::readNetFromDarknet("../models/yolov4.cfg", "../models/yolov4.weights");	
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

	// Get the output labels
	auto output_names = net.getUnconnectedOutLayersNames();

	// Read the frames to the last frame
	while (vid_capture.isOpened())
	{
		
	    // Initialize a boolean to check if frames are there or not
		bool isSuccess = vid_capture.read(frame);	
		    // Read picture
		//frame = cv::imread("../test_images/strange_nina.jpeg");
		if (frame.empty())
		{
			std::cout << "!!! Failed imread(): image not found" << std::endl;
			// don't let the execution continue, else imshow() will crash.
		}


		auto total_start = std::chrono::steady_clock::now();

		// If frames are present, show it		
		if(isSuccess == true)
		{
			
			//display frames
			//imshow("Frame", frame);
			if(imageWritten && frameCount == 600) 
			{
				/// -- Detect something in image. --
				// Setup image blob from the frame
				cv::dnn::blobFromImage(frame, blob, 0.00392, cv::Size(608, 608), cv::Scalar(), true, false, CV_32F);

				// Set the blob as the input for the neural net
				net.setInput(blob);

				// Start a forward pass and get the output detections.
				auto dnn_start = std::chrono::steady_clock::now();
				net.forward(detections, output_names);
				auto dnn_end = std::chrono::steady_clock::now();

				// for(cv::Mat detect : detections) 
				// {
				// 	std::cout << detect << std::endl;	
				// }									

				std::vector<int> indices[NUM_CLASSES];
				std::vector<cv::Rect> boxes[NUM_CLASSES];
				std::vector<float> scores[NUM_CLASSES];

				// Process detections and assign the boxes and the confidence number on the detection 
				for (auto& output : detections)
				{
					const auto num_boxes = output.rows;
					for (int i = 0; i < num_boxes; i++)
					{
						auto x = output.at<float>(i, 0) * frame.cols;
						auto y = output.at<float>(i, 1) * frame.rows;
						auto width = output.at<float>(i, 2) * frame.cols;
						auto height = output.at<float>(i, 3) * frame.rows;
						cv::Rect rect(x - width/2, y - height/2, width, height);

						for (int c = 0; c < NUM_CLASSES; c++)
						{
							auto confidence = *output.ptr<float>(i, 5 + c);
							if (confidence >= CONFIDENCE_THRESHOLD)
							{
								boxes[c].push_back(rect);
								scores[c].push_back(confidence);
							}
						}
					}
				}

				// Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences.
				// https://learnopencv.com/deep-learning-based-object-detection-using-yolov3-with-opencv-python-c/
				for (int c = 0; c < NUM_CLASSES; c++)
					cv::dnn::NMSBoxes(boxes[c], scores[c], 0.0, NMS_THRESHOLD, indices[c]);
				
				// Draw the rectangle with label and output the results to the console
				// passes the frame by reference?
				for (int c= 0; c < NUM_CLASSES; c++)
				{
					for (size_t i = 0; i < indices[c].size(); ++i)
					{
						const auto color = colors[c % NUM_COLORS];

						auto idx = indices[c][i];
						const auto& rect = boxes[c][idx];
						cv::rectangle(frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 3);

						std::ostringstream label_ss;
						label_ss << class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
						auto label = label_ss.str();

						std::cout << "class: " << class_names[c] << " score: " << scores[c][idx] << std::endl;
						
						int baseline;
						auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
						cv::rectangle(frame, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 10), cv::Point(rect.x + label_bg_sz.width, rect.y), color, cv::FILLED);
						cv::putText(frame, label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 0, 0));
					}
				}
			
				auto total_end = std::chrono::steady_clock::now();

				float inference_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(dnn_end - dnn_start).count();
				float total_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();

				// Get stats for the picture and draw them.
				std::ostringstream stats_ss;
				// stats_ss << std::fixed << std::setprecision(2);
				// stats_ss << "Inference FPS: " << inference_fps << ", Total FPS: " << total_fps;
				auto stats = stats_ss.str();
					
				int baseline;
				auto stats_bg_sz = cv::getTextSize(stats.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
				cv::rectangle(frame, cv::Point(0, 0), cv::Point(stats_bg_sz.width, stats_bg_sz.height + 10), cv::Scalar(0, 0, 0), cv::FILLED);
				cv::putText(frame, stats.c_str(), cv::Point(0, stats_bg_sz.height + 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255, 255, 255));

				cv::namedWindow("output");
				//cv::imshow("output", frame);
				cv::imwrite("./testing.jpg", frame);
				std::cout << "Finished writing the image" << std::endl;


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

		frameCount++;		
	}
	// Release the video capture object
	vid_capture.release();
	//destroyAllWindows();
	return 0;
}