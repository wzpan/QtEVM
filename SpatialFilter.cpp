#include "SpatialFilter.h"

/** 
 * buildLaplacianPyramid	-	construct a laplacian pyramid from given image
 *
 * @param img		-	source image
 * @param levels	-	levels of the destinate pyramids
 * @param pyramid	-	destinate image
 *
 * @return true if success
 */
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

/** 
 * buildGaussianPyramid	-	construct a gaussian pyramid from a given image
 *
 * @param img		-	source image
 * @param levels	-	levels of the destinate pyramids
 * @param pyramid	-	destinate image
 *
 * @return true if success
 */
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

/** 
 * reconImgFromLaplacianPyramid	-	reconstruct image from given laplacian pyramid
 *
 * @param pyramid	-	source laplacian pyramid
 * @param levels	-	levels of the pyramid
 * @param dst		-	destinate image
 */
void reconImgFromLaplacianPyramid(const std::vector<cv::Mat_<cv::Vec3f> > &pyramid,
                                  const int levels,
                                  cv::Mat_<cv::Vec3f> &dst)
{
    cv::Mat currentImg = pyramid[levels];
    for (int l=levels-1; l>=0; l--) {
        cv::Mat up;
        cv::pyrUp(currentImg, up, pyramid[l].size());
        currentImg = up + pyramid[l];
    }
    dst = currentImg.clone();
}

/** 
 * upsamplingFromGaussianPyramid	-	up-sampling an image from gaussian pyramid
 *
 * @param src		-	source image
 * @param levels	-	levels of the pyramid
 * @param dst		-	destinate image
 */
void upsamplingFromGaussianPyramid(const cv::Mat &src,
                                   const int levels,
                                   cv::Mat_<cv::Vec3f> &dst)
{
    cv::Mat currentLevel = src.clone();
    for (int i = 0; i < levels; ++i) {
        cv::Mat up;
        cv::pyrUp(currentLevel, up);
        currentLevel = up;
    }
    currentLevel.copyTo(dst);
}
