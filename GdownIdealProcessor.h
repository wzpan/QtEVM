#ifndef GDOWNIDEALPROCESSOR_H
#define GDOWNIDEALPROCESSOR_H

#include "FrameProcessor.h"
#include "Utils.h"

class GdownIdealProcessor : public FrameProcessor
{
    friend class GdownIdealDialog;

public:
    GdownIdealProcessor();

    // Spatial gaussian temporal ideal processing method
    void process(cv:: Mat &input, cv:: Mat &output);
    void reset();

private:
    int levels;
    float alpha;
    float fl;
    float fh;
    float chromAttenuation;

    void amplify_spatial_gdown_temporal_ideal(const cv::Mat &src, cv::Mat &dst);

};

#endif // GDOWNIDEAL_H
