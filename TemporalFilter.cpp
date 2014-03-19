#include "TemporalFilter.h"

/**
 * shiftDFT	-	Rearrange the quadrants of a Fourier image
 * so that the origin is at the image center
 *
 * @param fImage
 */
void shiftDFT(cv::Mat &fImage )
{
    cv::Mat tmp, q0, q1, q2, q3;

    // first crop the image, if it has an odd number of rows or columns

    fImage = fImage(cv::Rect(0, 0, fImage.cols & -2, fImage.rows & -2));

    int cx = fImage.cols / 2;
    int cy = fImage.rows / 2;

    // rearrange the quadrants of Fourier image
    // so that the origin is at the image center

    q0 = fImage(cv::Rect(0, 0, cx, cy));
    q1 = fImage(cv::Rect(cx, 0, cx, cy));
    q2 = fImage(cv::Rect(0, cy, cx, cy));
    q3 = fImage(cv::Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}


/**
 * create_spectrum_magnitude_display	-	return a floating point spectrum magnitude image
 * scaled for user viewing
 * fourier image)
 *
 * @param complexImg	-	input dft (2 channel floating point, Real + Imaginary
 * @param rearrange		-	perform rearrangement of DFT quadrants if true
 *
 * @return	- pointer to output spectrum magnitude image scaled for user viewing
 */
cv::Mat create_spectrum_magnitude_display(cv::Mat &complexImg, bool rearrange)
{
    cv::Mat planes[2];

    // compute magnitude spectrum (N.B. for display)
    // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))

    cv::split(complexImg, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);

    cv::Mat mag = (planes[0]).clone();
    mag += cv::Scalar::all(1);
    log(mag, mag);

    if (rearrange)
    {
        // re-arrange the quaderants
        shiftDFT(mag);
    }

    cv::normalize(mag, mag, 0, 1, CV_MINMAX);

    return mag;

}


/**
 * createIdealBandpassFilter	-	create a 1D ideal band-pass filter
 *
 * @param filter    -	destinate filter
 * @param fl        -	low cut-off
 * @param fh		-	high cut-off
 * @param rate      -   sampling rate(i.e. video frame rate)
 */
void createIdealBandpassFilter(cv::Mat &filter, double fl, double fh, double rate)
{
    int width = filter.cols;
    int height = filter.rows;

    fl = 2 * fl * width / rate;
    fh = 2 * fh * width / rate;

    cv::Mat tmp = cv::Mat(filter.rows, filter.cols, CV_32F);

    double response;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // filter response
            if (j >= fl && j <= fh)
                response = 1.0f;
            else
                response = 0.0f;
            tmp.at<float>(i, j) = response;
        }
    }

    cv::Mat toMerge[] = {tmp, tmp};
    cv::merge(toMerge, 2, filter);
}
