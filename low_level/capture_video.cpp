

#include <iostream>
#include <fstream>
#include <filesystem>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

#include <curl/curl.h>


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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
     std::cout << "Here 2" << std::endl;
    return size * nmemb;
}

int main () {

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

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Matrix for holding the image data
    cv::Mat frame;
    // Matrix for holding the image blob that will be used in the AI model
    cv::Mat blob;
    // Declaring a video player that will show the video
    cv::namedWindow("Video Player");
    // Declare object for capturing the video
    cv::VideoCapture cap(0);
    // Matix vector for AI detections
	std::vector<cv::Mat> detections;

	int frameCount = 0;
	bool imageWritten = true;

    if(!cap.isOpened()) {
        std::cout << "No video stream detected" << std::endl;
        std::system("pause");
        return -1;
    }

    // Initialize model.
	cv::dnn::Net net = cv::dnn::readNetFromDarknet("../models/yolov4.cfg", "../models/yolov4.weights");	
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

	// Get the output labels
	auto output_names = net.getUnconnectedOutLayersNames();

    //auto total_start = std::chrono::steady_clock::now();

    // Main loop for processing video frames
    while(true) {
        cap >> frame;
        // Stop the loop if no video frame is detected
        if (frame.empty()) {
            std::cout << "No video frames detected, stoping stream." << std::endl;
            break;
        }

        // Check detection every 120 frames
        if (frameCount % 480 == 0) {
            // Apply Yolo to frame

            // Setup image blob from the frame
            cv::dnn::blobFromImage(frame, blob, 0.00392, cv::Size(608, 608), cv::Scalar(), true, false, CV_32F);

            // Set the blob as the input for the neural net
            net.setInput(blob);

            // Start a forward pass and get the output detections.
            //auto dnn_start = std::chrono::steady_clock::now();
            net.forward(detections, output_names);
            //auto dnn_end = std::chrono::steady_clock::now();

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
            

            // Show how many detections 
            // Todo, put detection in hash map to allow for easier handling.
            std::cout << "Number of detections" << std::endl;
            for (int c= 0; c < NUM_CLASSES; c++)
            {
                if(indices[c].size())
                    std::cout << "class " << c << " has " << indices[c].size() << std::endl;
            }

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

            //float inference_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(dnn_end - dnn_start).count();
            //float total_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();

            // Get stats for the picture and draw them.
            std::ostringstream stats_ss;
            // stats_ss << std::fixed << std::setprecision(2);
            // stats_ss << "Inference FPS: " << inference_fps << ", Total FPS: " << total_fps;
            auto stats = stats_ss.str();
                
            int baseline;
            auto stats_bg_sz = cv::getTextSize(stats.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
            cv::rectangle(frame, cv::Point(0, 0), cv::Point(stats_bg_sz.width, stats_bg_sz.height + 10), cv::Scalar(0, 0, 0), cv::FILLED);
            cv::putText(frame, stats.c_str(), cv::Point(0, stats_bg_sz.height + 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255, 255, 255));

            //cv::namedWindow("output");
            //cv::imshow("output", frame);
            cv::imwrite("./testing2.jpg", frame);
            std::cout << "Finished writing the image" << std::endl;

            // Send the image via pushover
            std::string messageText = "Alert: a detection has been made";
            std::string imagePath = "./testing2.jpg";

            struct curl_httppost* formpost = NULL;
            struct curl_httppost* lastptr = NULL;

            curl_global_init(CURL_GLOBAL_ALL);

            curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "token",
                CURLFORM_COPYCONTENTS , "abc5kk96z5ave92xg8edp6cc89dnui",
                CURLFORM_END);
                
            curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "user",
                CURLFORM_COPYCONTENTS , "ur8ix1abhx1s8wn3s4s6m8dd3trbiu",
                CURLFORM_END);

            curl_formadd(&formpost,
                &lastptr,
                CURLFORM_COPYNAME, "message",
                CURLFORM_COPYCONTENTS , messageText.data(),
                CURLFORM_END);

            // Todo: send picture from memory instad of reading it from disk
            curl_formadd(&formpost, 
                &lastptr,
                CURLFORM_COPYNAME, "attachment",        
                CURLFORM_FILE, imagePath.data(),
                CURLFORM_CONTENTTYPE, "image/jpeg",
                CURLFORM_END);

            curl = curl_easy_init();
            if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "https://api.pushover.net/1/messages.json");

                //curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
                curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                res = curl_easy_perform(curl);        

                if (res != CURLE_OK) {
                    std::cout << "Call not succesfull " << std::endl;
                }

                std::cout << "Res: " << res << std::endl;
                std::cout << readBuffer << std::endl;
            }


            curl_formfree(formpost);
            std::cout << "Here" << std::endl;
            curl_easy_cleanup(curl);
            //curl_slist_free_all(headerlist);

            curl_global_cleanup();

        }

        cv::imshow("Video Player", frame);
        // Wait 25 milliseconds for frame processing
        char c = (char)cv::waitKey(25);

        // Exit if 'Esc' button is pressed
        if(c == 27) {
            break;
        }

        frameCount++;
    }

    // Release memory
    cap.release();
    return 0;
}