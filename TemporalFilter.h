#ifndef TEMPORALFILTER_H
#define TEMPORALFILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <math.h>

// rearrange
void shiftDFT(cv::Mat &fImage);

cv::Mat create_spectrum_magnitude_display(cv::Mat &complexImg, bool rearrange);

// create an ideal bandpass processor
void createIdealBandpassFilter(cv::Mat &filter, double fl, double fh, double rate);

#endif // TEMPORALFILTER_H
