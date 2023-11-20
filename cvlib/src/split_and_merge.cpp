/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 20238-10-23
 * @author Shipkov Nikita
 */

#include "cvlib.hpp"

namespace
{
struct RectangleRegion
{
    cv::Rect rectangle;
    double d;
    double m;

    bool compareD(const RectangleRegion& right, double stddev)
    {
        return (d < stddev) && (right.d < stddev);
    }

    bool isIntersect(const RectangleRegion& right)
    {
        auto test = cv::Rect(right.rectangle.x - 1, right.rectangle.y - 1, right.rectangle.width, right.rectangle.height);
        auto test2 = cv::Rect(right.rectangle.x + 1, right.rectangle.y + 1, right.rectangle.width, right.rectangle.height);

        if (this->rectangle.area() * right.rectangle.area() == 1)
        {
            return ((rectangle.x - right.rectangle.x) <= 1) && ((rectangle.y - right.rectangle.y) <= 1);
        }

        return ((rectangle & test).width > 0) || ((rectangle & test2).width > 0);
    }
};

struct ComplexRegion
{
    std::vector<RectangleRegion> rectangles;

    ComplexRegion(const RectangleRegion& rectangle)
    {
        rectangles.push_back(rectangle);
    }

    double d()
    {
        double s = 0;
        double m = this->m();
        int sumArea = 0;
        for (int i = 0; i < rectangles.size(); i++)
        {
            s += rectangles[i].rectangle.area()*pow(rectangles[i].m-m, 2);
            sumArea += rectangles[i].rectangle.area();
        }

        return sqrt(s / (sumArea - 1));

    }

    double m()
    {
        double m = 0;
        int sumArea = 0;
        for (int i = 0; i < rectangles.size(); i++)
        {
            m += rectangles[i].m*rectangles[i].rectangle.area();
            sumArea += rectangles[i].rectangle.area();
        }

        return m / sumArea;

    }

    void addRegions(const ComplexRegion& right)
    {
        rectangles.insert(rectangles.end(), right.rectangles.begin(), right.rectangles.end());
    }

    bool isIntersect(const ComplexRegion& right)
    {
        for (int i = 0; i < rectangles.size(); i++)
        {
            for (int j = 0; j < right.rectangles.size(); j++)
            {
                if (rectangles[i].isIntersect(right.rectangles[j]))
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool compareD(ComplexRegion& right, double stddev)
    {
        auto a = this->rectangles;
        
        this->addRegions(right);
        bool ans = (this->d() <= stddev);

        this->rectangles = a;

        return ans;
    }
};



std::list<ComplexRegion> listOfRegions;


void split_image(cv::Mat image, double stddev, cv::Rect rectangle)
{
    if (image.empty())
        return;
    
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        RectangleRegion region;
        region.d = dev.at<double>(0);
        region.m = mean.at<double>(0);
        region.rectangle = rectangle;
        listOfRegions.push_back(ComplexRegion(region));
        return;
    }

    const auto width = image.cols;
    const auto height = image.rows;


    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev, 
        cv::Rect(rectangle.x,rectangle.y,rectangle.width/2,rectangle.height/2));
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev,
        cv::Rect(rectangle.x+rectangle.width/2,rectangle.y,rectangle.width/2,rectangle.height/2));
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev,
                cv::Rect(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2, rectangle.width / 2, rectangle.height / 2));
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev,
        cv::Rect(rectangle.x, rectangle.y+rectangle.height/2, rectangle.width / 2, rectangle.height / 2));
}

void merge_image(cv::Mat image, double stddev)
{
    if (listOfRegions.empty())
        return;

    std::vector<ComplexRegion> result;

    ComplexRegion first = listOfRegions.front();
    listOfRegions.pop_front();



    for (auto it = listOfRegions.begin(); it != listOfRegions.end(); ++it)
    {
        if (first.compareD(*it,stddev))
            if (first.isIntersect(*it))
            {
                first.addRegions(*it);
                listOfRegions.push_back(first);
                result.push_back(first);
                listOfRegions.erase(it);
                first = listOfRegions.front();
                
                listOfRegions.pop_front();
                
                it = listOfRegions.begin();

            }
    }



    

    for (int i = 0; i < result.size(); i++)
    {
        auto reg = result[i];
        auto m = result[i].m();

        for (int j = 0; j < reg.rectangles.size(); j++)
        {
            
            auto image_roi = image(reg.rectangles[j].rectangle);

            image_roi.setTo(m);
        }
       
    }
}

} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    listOfRegions.clear();

    
    cv::Mat res = image;
    split_image(res, stddev,cv::Rect(0,0,res.size().width,res.size().height));
    merge_image(res, stddev);

    // merge part
    // \todo implement merge algorithm
    return res;
}
} // namespace cvlib
