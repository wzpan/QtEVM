#include "MorphologyProcessor.h"

MorphologyProcessor::MorphologyProcessor()
{
    type = 0;
    elem = 0;
    size = 3;
}

/** 
 * process	-	morphology processing method
 *
 * @param img	-	the input image
 * @param out	-	the output image
 */
void MorphologyProcessor :: process(cv::Mat &img, cv::Mat &out)
{
    cv::Mat element = cv::getStructuringElement( 0,
                                             cv::Size( 2*size + 1, 2*size+1 ),
                                             cv::Point( size, size ) );
    // Apply the erosion operation
    if (0 == type)
        cv::erode( img, out, element );
    else
        cv::dilate( img, out, element);
}

int MorphologyProcessor::getType()
{
    return type;
}

void MorphologyProcessor::setType(int t)
{
    type = t;
}

int MorphologyProcessor::getSize()
{
    return size;
}

int MorphologyProcessor::getElem()
{
    return elem;
}

void MorphologyProcessor::setSize(int s)
{
    size = s;
}

void MorphologyProcessor::setElem(int e)
{
    elem = e;
}

