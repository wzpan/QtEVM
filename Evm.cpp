//
//  Evm.cpp
//  example_evm
//
//  Created by Yusuke on 2013/03/30.
//
//

#include "Evm.h"
#include "Utils.h"

#include <dbg.h>

using namespace cv;
using namespace std;

Evm::Evm()
:levels(6)
,is_not_first_frame(false)
{
}

Evm::~Evm()
{
}

void Evm::reset()
{
    is_not_first_frame = false;
}

void Evm::amplify_spatial_lpyr_temporal_iir(const cv::Mat& src, vector<cv::Mat_<Vec3f> >& lapPyr, cv::Mat& dst,
                                            float alpha, float lambda_c, float r1, float r2, float chromAttenuation)
{
    cv::Mat_<Vec3f> s = src.clone();
    rgb2ntsc(src, s);

    buildLaplacianPyramid(s, lapPyr);

    if (is_not_first_frame) {
        //temporal iir
        for (int i=0; i<lapPyr.size(); i++) {
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

    dst = s + reconstructImgFromLapPyramid(filtered).mul(Vec3f(1,chromAttenuation,chromAttenuation));

    s = dst.clone();
    ntsc2rgb(s, s);
    dst = s.clone();
}


void Evm::amplify_spatial_gdown_temporal_ideal(const cv::Mat &src, std::vector<cv::Mat_<Vec3f> > &gPyr, cv::Mat &dst,
                                               float alpha, float lambda_c, float fl, float fh, float chromAttenuation)
{
    cv::Mat_<Vec3f> s = src.clone();
    rgb2ntsc(src, s);

    // Spacial filtiering
    buildGaussianPyramid(s, gPyr);

    // Temporal filtering
    ideal_bandpassing(gPyr, fl, fh);

    // amplify
    for (int i=0; i<static_cast<int>(gPyr.size()); ++i) {
        cv::Mat temp;
        std::vector<cv::Mat> planes;
        split(gPyr.at(i), planes);
        planes[0] = planes[0] * alpha;
        planes[1] = planes[1] * alpha * chromAttenuation;
        planes[2] = planes[2] * alpha * chromAttenuation;
        merge(planes, temp);
        resize(temp, temp, s.size());
        s = s + temp;
    }

    // s = dst.clone();
    ntsc2rgb(s, s);
    dst = s.clone();    
}


/**
 * ideal_bandpassing    -   band passing the pyramid
 *
 * @param pyr   -   source and destinate pyramid
 * @param fl    -   lower cutoff frequency of ideal band pass filter
 * @param fh    -   higher cutoff frequency of ideal band pass filter
 */
void Evm::ideal_bandpassing(std::vector<cv::Mat_<Vec3f> > &pyr, float fl, float fh)
{
    cv::Mat padded;		// fourier image objects and arrays
    cv::Mat complexImg, imgOutput, filter;
    cv::Mat planes[3], complexPlanes[2], mag;

    int M, N;	// fourier image sizes

    // for each level from pyramid
    for (int i=0; i<static_cast<int>(pyr.size()); ++i){

        // setup the DFT image sizes
        M = getOptimalDFTSize(pyr.at(i).rows);
        N = getOptimalDFTSize(pyr.at(i).cols);

        // split channels
        split(pyr.at(i), planes);

        // for each channel from one image level
        for (int j=0; j<3; ++j) {

            // setup the DFT images
            copyMakeBorder(planes[j], padded, 0, M - planes[j].rows, 0,
                           N - planes[j].cols, BORDER_CONSTANT, Scalar::all(0));
            complexPlanes[0] = cv::Mat_<float>(padded);
            complexPlanes[1] = cv::Mat::zeros(padded.size(), CV_32F);
            merge(complexPlanes, 2, complexImg);

            // do the DFT
            dft(complexImg, complexImg);

            // construct the filter (same size as complex image)
            filter = complexImg.clone();
            create_ideal_bandpass_filter(filter, fl, fh);

            // apply filter
            shiftDFT(complexImg);	// centerize
            mulSpectrums(complexImg, filter, complexImg, 0);
            shiftDFT(complexImg);

            // do inverse DFT on filtered image
            idft(complexImg, complexImg);

            // split into planes and extract plane 0 as output image
            cv::Mat myplanes[2];
            split(complexImg, myplanes);
            double minimum = -1;
            double maximum = -1;
            cv::Point minloc(-1, -1), maxloc(-1, -1);
            minMaxLoc(myplanes[0], &minimum, &maximum, &minloc, &maxloc);
            normalize(myplanes[0], imgOutput, 0, 1, CV_MINMAX);
            imgOutput = myplanes[0];
            planes[j] = imgOutput.clone();
        }

        // merge channels
        cv::Mat result;
        merge(planes, 3, result);

        pyr.at(i) = result.clone();
    }
}

/**
 * create_ideal_bandpass_filter	-	create a 2-channel ideal band-pass filter
 *		with cutoff frequencies fl and fh (assumes pre-aollocated size of dft_Filter specifies dimensions)
 *
 * @param dft_Filter	-	destinate filter
 * @param fl				-	radius
 * @param fh				-	band width
 */
void Evm::create_ideal_bandpass_filter(cv::Mat &dft_Filter, float fl, float fh)
{
    cv::Mat tmp = cv::Mat(dft_Filter.rows, dft_Filter.cols, CV_32F);

    Point centre = Point(dft_Filter.rows / 2, dft_Filter.cols / 2);
    double radius;

    for (int i = 0; i < dft_Filter.rows; i++)
    {
        for (int j = 0; j < dft_Filter.cols; j++)
        {
            // D(u, v)
            radius = (double) sqrt(pow((i - centre.x), 2.0) + pow((double) (j - centre.y), 2.0));

            if (radius >= fl && radius <= fh)
                tmp.at<float>(i, j) = 1;
            else
                tmp.at<float>(i, j) = 0;
        }
    }

    cv::Mat toMerge[] = {tmp, tmp};
    merge(toMerge, 2, dft_Filter);
}

/**
 * shiftDFT	-	Rearrange the quadrants of a Fourier image so that the origin is at
 *				the image center
 *
 * @param fImage	-	fourier image
 */
void Evm::shiftDFT(cv::Mat &fImage )
{
    cv::Mat tmp, q0, q1, q2, q3;

    // first crop the image, if it has an odd number of rows or columns

    fImage = fImage(Rect(0, 0, fImage.cols & -2, fImage.rows & -2));

    int cx = fImage.cols / 2;
    int cy = fImage.rows / 2;

    // rearrange the quadrants of Fourier image
    // so that the origin is at the image center

    q0 = fImage(Rect(0, 0, cx, cy));
    q1 = fImage(Rect(cx, 0, cx, cy));
    q2 = fImage(Rect(0, cy, cx, cy));
    q3 = fImage(Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}


/**
 * create_spectrum_magnitude_display
 *
 * return a floating point spectrum magnitude image scaled for user viewing
 *
 * @param complexImg	-	input dft (2 channel floating point, Real + Imaginary fourier image)
 * @param rearrange		-	perform rearrangement of DFT quadrants if true
 *
 * @return - pointer to output spectrum magnitude image scaled for user viewing
 */

cv::Mat Evm::create_spectrum_magnitude_display(cv::Mat &complexImg, bool rearrange)
{
    cv::Mat planes[2];

    // compute magnitude spectrum (N.B. for display)
    // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))

    split(complexImg, planes);
    magnitude(planes[0], planes[1], planes[0]);

    cv::Mat mag = (planes[0]).clone();
    mag += Scalar::all(1);
    log(mag, mag);

    if (rearrange)
    {
        // re-arrange the quaderants
        shiftDFT(mag);
    }

    normalize(mag, mag, 0, 1, CV_MINMAX);

    return mag;

}

void Evm::amplifyByAlpha(const cv::Mat& src, float alpha, float lambda_c)
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

void Evm::buildGaussianPyramid(const cv::Mat& img, vector<cv::Mat_<Vec3f> >& gPyr) {
    gPyr.clear();
    cv::Mat currentImg = img;
    for (int l=0; l<levels; l++) {
        cv::Mat down;
        pyrDown(currentImg, down);
        currentImg = down;
        gPyr.push_back(down);
    }
}


void Evm::buildLaplacianPyramid(const cv::Mat& img, vector<cv::Mat_<Vec3f> >& lapPyr) {
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

cv::Mat_<Vec3f> Evm::reconstructImgFromLapPyramid(vector<cv::Mat_<Vec3f> >& lapPyr) {
    cv::Mat currentImg = lapPyr[levels];
    for (int l=levels-1; l>=0; l--) {
        cv::Mat up;
        pyrUp(currentImg, up, lapPyr[l].size());
        currentImg = up + lapPyr[l];
    }
    return currentImg;
}

