#include "Engine.h"
#include <opencv2/opencv.hpp>



int main() {

    

    cv::Scalar lower_pink(130, 0, 130);
    cv::Scalar upper_pink(255, 255, 255);

    std::string video_path = "http://192.168.30.142:8443/normal.py";
    cv::VideoCapture cap(video_path);

    cv::Mat frame, hls_frame, mask;

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    

    Engine* engine = new Engine(frame_width, frame_height);


    bool running = true;

    while (running) {



        double relativ_x;
        double relativ_y;

        bool ret = cap.read(frame);

        if (!ret) {
            break;
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        cv::cvtColor(frame, hls_frame, cv::COLOR_BGR2HLS);

        //cv::flip(frame, frame, 1);

        cv::inRange(hls_frame, lower_pink, upper_pink, mask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            auto largest_contour = *std::max_element(contours.begin(), contours.end(),
                                                     [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                                                         return cv::contourArea(a) < cv::contourArea(b);
                                                     });

            cv::Rect bounding_rect = cv::boundingRect(largest_contour);

            cv::rectangle(frame, bounding_rect, cv::Scalar(0, 255, 0), 2);

            int center_x = bounding_rect.x + bounding_rect.width / 2;
            int center_y = bounding_rect.y + bounding_rect.height / 2;

            relativ_x = static_cast<double>(center_x) / frame_width;
            relativ_y = static_cast<double>(center_y) / frame_height;
                        
            std::cout << "Frame width: " << static_cast<double>(frame_width)/frame_height << ", Frame height: " << frame_height << std::endl;

            cv::circle(frame, cv::Point(center_x, center_y), 5, cv::Scalar(255, 0, 0), -1);

        }
        cv::imshow("Mask", mask);

        double engine_x = relativ_x - 0.5;
        double engine_y = relativ_y - 0.5;



        engine->renderer->window.cubeSystemTranslationY = (10000 - (engine_x*4500));
        engine->renderer->window.cubeSystemTranslationZ = (10000 - (engine_y*(6000)*(static_cast<double>(frame_width)/frame_height)));


        int key = cv::waitKey(10);


        engine->run(key, frame);
        
        if (key == 27) {
            running = false;
        
        }
    }


    cap.release();
    cv::destroyAllWindows();


    delete engine;


    return 0;
}