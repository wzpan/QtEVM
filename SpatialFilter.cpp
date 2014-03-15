#include "SpatialFilter.h"

bool buildLaplacianPyramid(const cv::Mat &img, const int levels,
                           std::vector<cv::Mat_<cv::Vec3f> > &pyramid)
{
    if (levels < 1){
        perror("Levels should be larger than 1");
        return false;
    }
    pyramid.clear();
    cv::Mat currentImg = img;
    for (int l=0; l<levels; l++) {
        cv::Mat down,up;
        pyrDown(currentImg, down);
        pyrUp(down, up, currentImg.size());
        cv::Mat lap = currentImg - up;
        pyramid.push_back(lap);
        currentImg = down;
    }
    pyramid.push_back(currentImg);
    return true;
}

bool buildGaussianPyramid(const cv::Mat &img,
                          const int levels,
                          std::vector<cv::Mat_<cv::Vec3f> > &pyramid)
{
    if (levels < 1){
        perror("Levels should be larger than 1");
        return false;
    }
    pyramid.clear();
    cv::Mat currentImg = img;
    for (int l=0; l<levels; l++) {
        cv::Mat down;
        cv::pyrDown(currentImg, down);        
        pyramid.push_back(down);
        currentImg = down;
    }
    return true;
}

void reconImgFromLaplacianPyramid(const std::vector<cv::Mat_<cv::Vec3f> > &pyramid,
                                  const int levels,
                                  cv::Mat &dst)
{
    dst = pyramid[levels];
    for (int l=levels-1; l>=0; l--) {
        cv::Mat up;
        cv::pyrUp(dst, up, pyramid[l].size());
        dst = up + pyramid[l];
    }
}
