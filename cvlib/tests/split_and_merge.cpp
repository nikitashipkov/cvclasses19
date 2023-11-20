/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("constant image", "[split_and_merge]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar{15});

    const auto res = split_and_merge(image, 1);
    REQUIRE(image.size() == res.size());
    REQUIRE(image.type() == res.type());
    REQUIRE(cv::Scalar(15) == cv::mean(res));
}

TEST_CASE("simple regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 2, 2, 2, 2);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 0, 1, 2, 3);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 5, 5, 5, 5, 5, 5, 5,5,5 );
        cv::Mat image = (cv::Mat_<char>(3, 3) << 1, 2, 3, 4, 5, 6, 7, 8, 9);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}

TEST_CASE("compex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 7, 7, 90, 90);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 6, 8, 90, 90);


        
        auto res = split_and_merge(image, 40);
        printf("%d", res.at<char>(0,0));
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
        
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 40, 4, 4, 40, 4, 4, 40, 40, 40);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 40, 4, 4, 40, 4, 4, 40, 40, 40);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) << 4, 4, 100, 100, 
                                                           4, 4, 100, 100, 
                                                           100, 100, 4, 4, 
                                                           100, 100, 4, 4);
        cv::Mat image = (cv::Mat_<char>(4, 4) << 2, 3, 100, 100, 
                                                 4, 7, 100, 100, 
                                                 100, 100, 2, 3, 
                                                 100, 100, 4, 7);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}
