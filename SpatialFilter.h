// Yet anther C++ implementation of EVM, based on OpenCV and Qt. 
// Copyright (C) 2014  Joseph Pan <cs.wzpan@gmail.com>
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301 USA
// 

#ifndef SPATIALFILTER_H
#define SPATIALFILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

// build a gaussian pyramid
bool buildGaussianPyramid(const cv::Mat &img, const int levels,
                           std::vector<cv::Mat> &pyramid);

// build a laplacian pyramid
bool buildLaplacianPyramid(const cv::Mat &img, const int levels,
                           std::vector<cv::Mat> &pyramid);

// reconstruct an image from a laplacian pyramid
void reconImgFromLaplacianPyramid(const std::vector<cv::Mat> &pyramid, const int levels,
                                  cv::Mat &dst);

// up-sampling an image from gaussian pyramid
void upsamplingFromGaussianPyramid(const cv::Mat &src, const int levels,
                                   cv::Mat &dst);

#endif // SPATIALFILTER_H
