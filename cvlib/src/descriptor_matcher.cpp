/* Descriptor matcher algorithm implementation.
 * @file
 * @date 2018-11-25
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace cvlib
{
void descriptor_matcher::knnMatchImpl(cv::InputArray queryDescriptors, std::vector<std::vector<cv::DMatch>>& matches, int k /*unhandled*/,
                                      cv::InputArrayOfArrays masks /*unhandled*/, bool compactResult /*unhandled*/)
{
    if (trainDescCollection.empty())
        return;

    auto q_desc = queryDescriptors.getMat();
    auto& t_desc = trainDescCollection[0];

    matches.resize(q_desc.rows);

    //cv::RNG rnd;

    for (int i = 0; i < q_desc.rows; ++i)
    {
        // \todo implement Ratio of SSD check.
       // matches[i].emplace_back(i, rnd.uniform(0, t_desc.rows), FLT_MAX);

        

        for (int j = 0; j < t_desc.rows; j++)
        {
            int dist = distance(q_desc.row(i), t_desc.row(j));
            if (dist < ratio_)
            {
                
                matches[i].emplace_back(i, j, dist);
            }
        }


    }
}

int descriptor_matcher::distance(const cv::Mat q_desc, const cv::Mat t_desc)
{
    int dist = 0;

    

    for (int i = 0; i < q_desc.cols; i++)
    {
        auto q_bytes = q_desc.at<int>(0, i);
        auto t_bytes = t_desc.at<int>(0, i);
        for (int j = 0; j < sizeof(int) * 8; j++)
        {
            auto q_bit = (q_bytes >> j) & 1;
            auto t_bit = (t_bytes >> j) & 1;
            if (q_bit != t_bit)
            {
                dist++;
            }
        }
    }
    return dist;
}






void descriptor_matcher::radiusMatchImpl(cv::InputArray queryDescriptors, std::vector<std::vector<cv::DMatch>>& matches, float /*maxDistance*/,
                                         cv::InputArrayOfArrays masks /*unhandled*/, bool compactResult /*unhandled*/)
{
    // \todo implement matching with "maxDistance"
    knnMatchImpl(queryDescriptors, matches, 1, masks, compactResult);
}
} // namespace cvlib
