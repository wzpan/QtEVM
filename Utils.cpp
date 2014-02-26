//
//  Utils.cpp
//  example_evm
//
//  Created by Yusuke on 2013/04/05.
//
//

#include "Utils.h"
using namespace cv;

void rgb2ntsc(const Mat_<Vec3f>& src, Mat_<Vec3f>& dst)
{
    Mat ret = src.clone();
    Mat T = (Mat_<float>(3,3) << 1, 1, 1, 0.956, -0.272, -1.106, 0.621, -0.647, 1.703);
    T = T.inv(); //here inverse!
    
    for (int j=0; j<src.rows; j++) {
        for (int i=0; i<src.cols; i++) {
            ret.at<Vec3f>(j,i)(0) = src.at<Vec3f>(j,i)(0) * T.at<float>(0.0)
            + src.at<Vec3f>(j,i)(1) * T.at<float>(0,1)
            + src.at<Vec3f>(j,i)(2) * T.at<float>(0,2);
            
            ret.at<Vec3f>(j,i)(1) = src.at<Vec3f>(j,i)(0) * T.at<float>(1.0)
            + src.at<Vec3f>(j,i)(1) * T.at<float>(1,1)
            + src.at<Vec3f>(j,i)(2) * T.at<float>(1,2);
            
            ret.at<Vec3f>(j,i)(2) = src.at<Vec3f>(j,i)(0) * T.at<float>(2.0)
            + src.at<Vec3f>(j,i)(1) * T.at<float>(2,1)
            + src.at<Vec3f>(j,i)(2) * T.at<float>(2,2);
        }
    }
    dst = ret;
}

void ntsc2rgb(const Mat_<Vec3f>& src, Mat_<Vec3f>& dst)
{
    Mat ret = src.clone();
    Mat T = (Mat_<float>(3,3) << 1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.106, 1.703);
    T = T.t(); //here transpose!
    
    for (int j=0; j<src.rows; j++) {
        for (int i=0; i<src.cols; i++) {
            ret.at<Vec3f>(j,i)(0) = src.at<Vec3f>(j,i)(0) * T.at<float>(0.0)
            + src.at<Vec3f>(j,i)(1) * T.at<float>(0,1)
            + src.at<Vec3f>(j,i)(2) * T.at<float>(0,2);
            
            ret.at<Vec3f>(j,i)(1) = src.at<Vec3f>(j,i)(0) * T.at<float>(1.0)
            + src.at<Vec3f>(j,i)(1) * T.at<float>(1,1)
            + src.at<Vec3f>(j,i)(2) * T.at<float>(1,2);
            
            ret.at<Vec3f>(j,i)(2) = src.at<Vec3f>(j,i)(0) * T.at<float>(2.0)
            + src.at<Vec3f>(j,i)(1) * T.at<float>(2,1)
            + src.at<Vec3f>(j,i)(2) * T.at<float>(2,2);
        }
    }
    dst = ret;
}

