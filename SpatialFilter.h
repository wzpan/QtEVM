#ifndef SPATIALFILTER_H
#define SPATIALFILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

// build a gaussian pyramid
bool buildGaussianPyramid(const cv::Mat &img, const int levels,
                           std::vector<cv::Mat_<cv::Vec3f> > &pyramid);

// build a laplacian pyramid
bool buildLaplacianPyramid(const cv::Mat &img, const int levels,
                           std::vector<cv::Mat_<cv::Vec3f> > &pyramid);

// reconstruct an image from a laplacian pyramid
void reconImgFromLaplacianPyramid(const std::vector<cv::Mat_<cv::Vec3f> > &pyramid, const int levels,
                                  cv::Mat &dst);

// up-sampling an image from gaussian pyramid
void upsamplingFromGaussianPyramid(const cv::Mat_<cv::Vec3f> &src, const int levels,
                                   cv::Mat &dst);

#endif // SPATIALFILTER_H
