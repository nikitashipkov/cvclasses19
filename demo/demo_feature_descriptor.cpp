/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "opencv2/imgproc.hpp"
#include "utils.hpp"
#include <cvlib.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>

int demo_feature_descriptor(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    cv::Mat frame;
    auto detector_a = cvlib::corner_detector_fast::create();
    auto detector_b = cv::SIFT::create();
    std::vector<cv::KeyPoint> corners;
    cv::Mat descriptors_a, descriptors_b;

    utils::fps_counter fps;
    int pressed_key = 0;
    while (pressed_key != 27) // ESC
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);

        detector_a->detect(frame, corners);
        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));

        utils::put_fps_text(frame, fps);
        // \todo add count of the detected corners at the top left corner of the image. Use green text color.
        
        cv::putText(frame, std::to_string(corners.size()), cv::Point(10, 20), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 255, 0));
        cv::imshow(demo_wnd, frame);

        pressed_key = cv::waitKey(30);
        // \todo draw histogram of SSD distribution for all descriptors instead of dumping into the file
        if (pressed_key == ' ') // space
        {

            detector_a->compute(frame, corners, descriptors_a);
            detector_b->compute(frame, corners, descriptors_b);


            descriptors_a.convertTo(descriptors_a, descriptors_b.type());



            bool test = descriptors_a.type() == descriptors_b.type();

  
            cv::Mat desc_xor;

            cv::bitwise_xor(descriptors_a, descriptors_b, desc_xor);
            desc_xor.convertTo(desc_xor, CV_32F);
            cv::Mat hist;
            int channels[] = {0};
            int histSize[] = {256};
            float sranges[] = {0, 256};
            const float* range[] = {sranges}; // the upper boundary is exclusive
            cv::calcHist(&desc_xor, 1, channels, cv::Mat(), // do not use mask
                         hist, 1, histSize, range,
                         true, // the histogram is uniform
                         false);

            int hist_w = 512, hist_h = 400;
            int bin_w = cvRound((double)hist_w / histSize[0]);
            cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

            cv::normalize(hist, hist, 0, desc_xor.rows, cv::NORM_MINMAX, -1, cv::Mat());

            std::cout << "hist: " << hist << std::endl;
            for (int i = 1; i < histSize[0]; i++)
            {
                cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                         cv::Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))), cv::Scalar(255, 0, 0), 2, 8, 0);
            }
            
            imshow("Histogramm of descriptors distanses", histImage);
        }


        std::cout << "Feature points: " << corners.size() << "\r";
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}