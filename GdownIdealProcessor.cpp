#include "GdownIdealProcessor.h"

GdownIdealProcessor::GdownIdealProcessor()
    : levels(6)
    , alpha(10)
    , fl(2.33)
    , fh(2.66)
    , chromAttenuation(0.1)
{
}

void GdownIdealProcessor::amplify_spatial_gdown_temporal_ideal(const cv::Mat &src, cv::Mat &dst)
{
    // TODO
}
