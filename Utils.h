//
//  Utils.h
//  example_evm
//
//  Created by Yusuke on 2013/04/05.
//
//

#ifndef __example_evm__Utils__
#define __example_evm__Utils__

#include <opencv2/core/core.hpp>

void rgb2ntsc(const cv::Mat_<cv::Vec3f>& src, cv::Mat_<cv::Vec3f>& dst);
void ntsc2rgb(const cv::Mat_<cv::Vec3f>& src, cv::Mat_<cv::Vec3f>& dst);

#endif /* defined(__example_evm__Utils__) */
