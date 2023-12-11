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

    std::vector<cv::Point> circle(16);

    for (int i = 3; i < _image.rows - 3; i++)
    {
        for (int j = 3; j < _image.cols - 3; j++)
        {          
           circle[0] = cv::Point(j, i - 3); 
           circle[4] = cv::Point(j + 3, i);
           circle[8] = cv::Point(j, i + 3);
           circle[12] = cv::Point(j - 3, i);
         
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
                circle[1] = cv::Point(j + 1, i - 3);
                circle[2] = cv::Point(j + 2, i - 2);
                circle[3] = cv::Point(j + 3, i - 1);
                circle[5] = cv::Point(j + 3, i + 1);
                circle[6] = cv::Point(j + 2, i + 2);
                circle[7] = cv::Point(j + 1, i + 3);
                circle[9] = cv::Point(j - 1, i + 3);
                circle[10] = cv::Point(j - 2, i + 2);
                circle[11] = cv::Point(j - 3, i + 1);
                circle[13] = cv::Point(j - 3, i - 1);
                circle[14] = cv::Point(j - 2, i - 2);
                circle[15] = cv::Point(j - 1, i - 3);

                count_dark = 0;
                count_light = 0;

                for (int k = 0; k < circle.size() + N - 1; k++)
                {
                    if (_image.at<uchar>(circle[k % 16]) > (currentPixel + t))
                    {
                        count_light++;
                        if (count_light >= N)
                        {
                            keypoints.emplace_back(cv::Point(j - 3, i - 3), 3);
                            break;
                        }
                        count_dark = 0;
                    }
                    else if (_image.at<uchar>(circle[k % 16]) < (currentPixel - t))
                    {
                        count_dark++;
                        if (count_dark >= N)
                        {
                            keypoints.emplace_back(cv::Point(j - 3, i - 3), 3);
                            break;
                        }
                        count_light = 0;
                    }
                    else
                    {
                        count_dark = 0;
                        count_light = 0;
                    }
                }
            }
            
            
        }
    }


    
    // \todo implement FAST with minimal LOCs(lines of code), but keep code readable.
}

void corner_detector_fast::compute(cv::InputArray inputArr, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    
    cv::Mat image = inputArr.getMat().clone();
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

    const int window = 10;
    const int desc_length = 128;
    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_32S);
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);


    cv::RNG generator(cv::getCPUTickCount());
    std::vector<std::pair<cv::Point2f, cv::Point2f>> offsets(desc_length);

    for (int i = 0; i < offsets.size(); i++)
    {
        std::pair<cv::Point2f, cv::Point2f> ptPair;
        ptPair.first = cv::Point2f(generator.uniform(-window / 2, window / 2), generator.uniform(-window / 2, window / 2));
        ptPair.second = cv::Point2f(generator.uniform(-window / 2, window / 2), generator.uniform(-window / 2, window / 2));
        offsets[i] = ptPair;
    }
    

    cv::copyMakeBorder(image, image, window / 2, window / 2, window / 2, window / 2, cv::BORDER_REPLICATE);

    int* ptr = reinterpret_cast<int*>(desc_mat.ptr());

    const int bitSize = sizeof(int) * 8;

     for (const auto& pt : keypoints)
    {
        for (int i = 0; i < desc_length / bitSize; ++i)
        {
            *ptr = 0;
            for (int j = 0; j < bitSize; j++)
            {
                uint8_t first_pix = image.at<uint8_t>(pt.pt + offsets[i * bitSize + j].first + cv::Point2f(window / 2, window / 2));
                uint8_t second_pix = image.at<uint8_t>(pt.pt + offsets[i * bitSize + j].second + cv::Point2f(window / 2, window / 2));
                *ptr |= (first_pix < second_pix) << (bitSize - 1 - j);
            }
            
            ++ptr;
        }
    }


}

void corner_detector_fast::detectAndCompute(cv::InputArray image, cv::InputArray mask, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors, bool /*= false*/)
{
    // \todo implement me
    
}
} // namespace cvlib
