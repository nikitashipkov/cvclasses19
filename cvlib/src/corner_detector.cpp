/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <ctime>

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}


void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();

    int t = 25;
    int N = 11;
    auto _image = image.getMat().clone();
    
    cv::cvtColor(_image, _image, cv::COLOR_BGR2GRAY);
    cv::copyMakeBorder(_image, _image, 3, 3, 3, 3, cv::BORDER_REPLICATE);

    std::vector<cv::Point> circle(15);

    for (int i = 3; i < _image.rows-3; i++)
    {
        for (int j = 3; j < _image.cols-3; j++)
        {          
           circle[0]=(cv::Point(j+1, i - 3)); 
           circle[4]=(cv::Point(j + 3, i));
           circle[8]=(cv::Point(j, i + 3));
           circle[12]=(cv::Point(j - 3, i));
         
            uchar currentPixel = _image.at<uchar>(i, j);

            int count_light = 0;
            int count_dark = 0;

            for (int k = 0; k < 13; k += 4)
            {
                if (_image.at<uchar>(circle[k]) > (currentPixel + t))
                {
                    count_light++;
                }
                else if (_image.at<uchar>(circle[k]) < (currentPixel - t))
                {
                    count_dark++;
                }
                
            }

            if ((count_dark >= 3) || (count_light >= 3))
            {
                circle[1] = (cv::Point(j + 1, i - 3));
                circle[2] = (cv::Point(j + 2, i - 2));
                circle[3] = (cv::Point(j + 3, i - 1));
                circle[5] = (cv::Point(j + 3, i + 1));
                circle[6] = (cv::Point(j + 2, i + 2));
                circle[7] = (cv::Point(j + 1, i + 3));
                circle[9] = (cv::Point(j - 1, i + 3));
                circle[10] = (cv::Point(j - 2, i + 2));
                circle[11] = (cv::Point(j - 3, i + 1));
                circle[13] = (cv::Point(j - 3, i - 1));
                circle[14] = (cv::Point(j - 2, i - 2));

                count_dark = 0;
                count_light = 0;
                for (int k = 0; k < circle.size(); k++)
                {
                    if (_image.at<uchar>(circle[k]) > (currentPixel + t))
                    {
                        count_light++;
                        if (count_light >= N)
                        {
                            keypoints.emplace_back(cv::Point(j, i), 3);
                            break;
                        }
                        count_dark = 0;
                    }
                    else if (_image.at<uchar>(circle[k]) < (currentPixel - t))
                    {
                        count_dark++;
                        if (count_dark >= N)
                        {
                            keypoints.emplace_back(cv::Point(j, i), 3);
                            break;
                        }
                        count_light = 0;
                    }
                }
            }
            
            
        }
    }


    
    // \todo implement FAST with minimal LOCs(lines of code), but keep code readable.
}

void corner_detector_fast::compute(cv::InputArray, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    std::srand(unsigned(std::time(0))); // \todo remove me
    // \todo implement any binary descriptor
    const int desc_length = 2;
    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_32S);
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);

    int* ptr = reinterpret_cast<int*>(desc_mat.ptr());
    for (const auto& pt : keypoints)
    {
        for (int i = 0; i < desc_length; ++i)
        {
            *ptr = std::rand();
            ++ptr;
        }
    }
}

void corner_detector_fast::detectAndCompute(cv::InputArray image, cv::InputArray mask, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors, bool /*= false*/)
{
    // \todo implement me
    
}
} // namespace cvlib
