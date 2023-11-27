/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{

motion_segmentation::motion_segmentation() : _threshold(100)
{
}

void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
    // \todo implement your own algorithm:
   
    //       * Mean
    

    
    
    cv::Mat image = _image.getMat().clone();
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    _frameBuffer.push_back(image);

    if (_frameBuffer.size()>5)
    {
        cv::Mat avgImg = _frameBuffer.front();
        _frameBuffer.pop_front();


        for (auto& i : _frameBuffer)
        {
            cv::accumulate(i, avgImg);
        }

        avgImg /= _frameBuffer.size() + 1;
        bg_model_ = avgImg;

        for (int i = 0; i < image.rows; i++)
            for (int j = 0; j < image.cols; j++)
            {
                if ((std::abs(image.at<uchar>(i, j) - avgImg.at<uchar>(i, j)))> _threshold)
                {
                    _fgmask.getMat().at<uchar>(i, j) = 255;
                }
                else
                {
                    _fgmask.getMat().at<uchar>(i, j) = 0;
                }
            }
    }




    // \todo implement bg model updates
}
} // namespace cvlib
