//
//  Evm.h
//  example_evm
//
//  Created by Yusuke on 2013/03/30.
//
//

#ifndef __example_evm__Evm__
#define __example_evm__Evm__

#include <opencv2/opencv.hpp>

class Evm
{
public:
    Evm();
    ~Evm();
    
    void reset();
    void amplify_spatial_lpyr_temporal_iir(const cv::Mat& img, std::vector<cv::Mat_<cv::Vec3f> >& lapPyr, cv::Mat& dst,
                                           float alpha, float lambda_c, float r1, float r2, float chromAttenuation);
    void amplify_spatial_gdown_temporal_ideal(const cv::Mat &src, std::vector<cv::Mat_<cv::Vec3f> > &gPyr, cv::Mat &dst, float alpha, float lambda_c, float fl, float fh, float chromAttenuation);
    
  
private:
    int levels;
    int is_not_first_frame;
    
    std::vector<cv::Mat_<cv::Vec3f> > lowpass1;
    std::vector<cv::Mat_<cv::Vec3f> > lowpass2;
    std::vector<cv::Mat_<cv::Vec3f> > filtered;

    void amplifyByAlpha(const cv::Mat& src, float alpha, float lambda_c);

    void ideal_bandpassing(std::vector<cv::Mat_<cv::Vec3f> >& pyr, float fl, float fh);

    void buildGaussianPyramid(const cv::Mat& img, std::vector<cv::Mat_<cv::Vec3f> >& gPyr);
    void buildLaplacianPyramid(const cv::Mat& img, std::vector<cv::Mat_<cv::Vec3f> >& lapPyr);
    
    cv::Mat_<cv::Vec3f> reconstructImgFromLapPyramid(std::vector<cv::Mat_<cv::Vec3f> >& lapPyr);

    void create_ideal_bandpass_filter(cv::Mat &dft_Filter, float fl, float fh);

    void shiftDFT(cv::Mat &fImage);

    cv::Mat create_spectrum_magnitude_display(cv::Mat &complexImg, bool rearrange);

};

#endif /* defined(__example_evm__Evm__) */
