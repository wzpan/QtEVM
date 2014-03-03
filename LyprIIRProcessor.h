#ifndef LYPRIIRPROCESSOR_H
#define LYPRIIRPROCESSOR_H

#include "FrameProcessor.h"
#include "Utils.h"

// The morphology processor class
class LyprIIRProcessor : public FrameProcessor
{
    friend class LyprIIRDialog;

public:
    LyprIIRProcessor(int levels=6);

    // Spatial laplacian temporal IIR processing method
    void process(const cv::Mat &img, cv::Mat &out);
    void reset();

private:
    int levels;
    double samplingRate;
    bool is_not_first_frame;
    float alpha;
    float lambda_c;
    float r1;
    float r2;
    float chromAttenuation;

    std::vector<cv::Mat_<cv::Vec3f> > lapPyr;
    std::vector<cv::Mat_<cv::Vec3f> > lowpass1;
    std::vector<cv::Mat_<cv::Vec3f> > lowpass2;
    std::vector<cv::Mat_<cv::Vec3f> > filtered;

    void amplify_spatial_lpyr_temporal_iir(const cv::Mat &src, cv::Mat &dst);

    void amplifyByAlpha(const cv::Mat& src, float alpha, float lambda_c);

    void buildLaplacianPyramid(const cv::Mat& img, std::vector<cv::Mat_<cv::Vec3f> > &lapPyr);

    cv::Mat_<cv::Vec3f> reconstructImgFromLapPyramid(std::vector<cv::Mat_<cv::Vec3f> > &lapPyr);
};

#endif // LYPRIIRPROCESSOR_H
