/**
 * @file   EVMLyprIIRProcessor.cpp
 * @author Joseph Pan <cs.wzpan@gmail.com>
 * @date   Wed Feb 26 22:32:45 2014
 * 
 * @brief  Laplacian Pyramid with IIR filters
 *
 * More info: http://people.csail.mit.edu/mrub/vidmag/
 * 
 */


#include "LyprIIRProcessor.h"

LyprIIRProcessor::LyprIIRProcessor()
    : levels(6)
    , is_not_first_frame(false)
    , alpha(10)
    , lambda_c(16)
    , r1(0.4)
    , r2(0.05)
    , chromAttenuation(0.1)
{
}

/** 
 * process	-	eulerian video magnification processing method
 *
 * @param img	-	input image
 * @param out	-	output image
 */
void LyprIIRProcessor :: process(const cv::Mat &img, cv::Mat &out)
{
    cv::Mat mat32f;
    img.convertTo(mat32f, CV_32F, 1.0/255.0);
    amplify_spatial_lpyr_temporal_iir(mat32f, out);
}

void LyprIIRProcessor::reset()
{
    is_not_first_frame = false;
}

/** 
 * amplify_spatial_lpyr_temporal_iir	-	laplacian pyramid with IIR temporal filter
 * 
 * Spatial Filtering: Laplacian pyramid
 * Temporal Filtering: substraction of two IIR lowpass filters
 *
 * y1[n] = r1*x[n] + (1-r1)*y1[n-1]
 * y2[n] = r2*x[n] + (1-r2)*y2[n-1]
 * (r1 > r2)
 *
 * y[n] = y1[n] - y2[n]
 *
 * @param src		-	source image
 * @param dst		-	output image
 */
void LyprIIRProcessor::amplify_spatial_lpyr_temporal_iir(const cv::Mat &src,
                                                            cv::Mat &dst)
{
    cv::Mat_<cv::Vec3f> s = src.clone();
    rgb2ntsc(src, s);

    buildLaplacianPyramid(s, lapPyr);

    if (is_not_first_frame) {
        //temporal iir
        for (int i=0; i<levels; i++) {
            cv::Mat temp1 = (1-r1)*lowpass1[i] + r1*lapPyr[i];
            cv::Mat temp2 = (1-r2)*lowpass2[i] + r2*lapPyr[i];
            lowpass1[i] = temp1;
            lowpass2[i] = temp2;
            filtered[i] = lowpass1[i] - lowpass2[i];
        }
        //amplify
        amplifyByAlpha(s, alpha, lambda_c);
    } else { // first frame
        lowpass1 = lapPyr;
        lowpass2 = lapPyr;
        filtered = lapPyr;
        is_not_first_frame = true;
    }

    cv::Mat tmp = reconstructImgFromLapPyramid(filtered);
    cv::Mat planes[3];
    cv::split(tmp, planes);
    planes[1] = planes[1] * chromAttenuation;
    planes[2] = planes[2] * chromAttenuation;
    cv::merge(planes, 3, tmp);
    dst = s + tmp;

    s = dst.clone();
    ntsc2rgb(s, s);
    dst = s.clone();

    double minVal, maxVal;
    minMaxLoc(dst, &minVal, &maxVal); //find minimum and maximum intensities
    dst.convertTo(dst, CV_8UC3, 255.0/(maxVal - minVal),
                  -minVal * 255.0/(maxVal - minVal));
}


/** 
 * amplifyByAlpha	-	caculate amplification parameters
 *
 * @param src	-	source image
 * @param alpha -	amplification factor
 * @param lambda_c	-	cut-off Wavelength
 */
void LyprIIRProcessor::amplifyByAlpha(const cv::Mat &src, float alpha, float lambda_c)
{
    int w = src.cols;
    int h = src.rows;

    //amplify each spatial frequency bands according to Figure 6 of paper
    float delta = lambda_c/8.0/(1.0+alpha);
    // the factor to boost alpha above the bound shown in the paper.
    // (for better visualization)
    float exaggeration_factor = 2.0;

    //compute the representative wavelength lambda for the lowest spatial
    // frequency band of Laplacian pyramid

    float lambda = sqrt(w*w + h*h)/3; // 3 is experimental constant

    for (int l=levels; l>=0; l--) {
        //compute modified alpha for this level
        float currAlpha = lambda/delta/8 - 1;
        currAlpha *= exaggeration_factor;
        if (l==levels || l==0) {  // ignore the highest and lowest frequency band
            filtered[l] = filtered[l] * 0;
        }
        else if (currAlpha > alpha) {	// representative lambda exceeds lambda_c
            filtered[l] = filtered[l] * alpha;
        }
        else {
            filtered[l] = filtered[l] * currAlpha;
        }
        //go one level down on pyramid
        // representative lambda will reduce by factor of 2
        lambda /= 2.0;
    }
}

/** 
 * buildLaplacianPyramid	-	build a laplacian pyramid from input image
 *
 * @param img		-	source image
 * @param lapPyr	-	output laplacian pyramid
 */
void LyprIIRProcessor::buildLaplacianPyramid(const cv::Mat &img,
                                                std::vector<cv::Mat_<cv::Vec3f> > &lapPyr)
{
    lapPyr.clear();
    cv::Mat currentImg = img;
    for (int l=0; l<levels; l++) {
        cv::Mat down,up;
        pyrDown(currentImg, down);
        pyrUp(down, up, currentImg.size());
        cv::Mat lap = currentImg - up;
        lapPyr.push_back(lap);
        currentImg = down;
    }
    lapPyr.push_back(currentImg);
}

/** 
 * reconstructImgFromLapPyramid	-	reconstruct image from a laplacian pyramid
 *
 * @param lapPyr	-	source laplacian pyramid
 *
 * @return	-	output image
 */
cv::Mat_<cv::Vec3f> LyprIIRProcessor::reconstructImgFromLapPyramid(
    std::vector<cv::Mat_<cv::Vec3f> > &lapPyr)
{
    cv::Mat currentImg = lapPyr[levels];
    for (int l=levels-1; l>=0; l--) {
        cv::Mat up;
        pyrUp(currentImg, up, lapPyr[l].size());
        currentImg = up + lapPyr[l];
    }
    return currentImg;
}

