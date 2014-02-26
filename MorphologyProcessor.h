#ifndef MORPHOPROCESSOR_H
#define MORPHOPROCESSOR_H

#include "FrameProcessor.h"

// The morphology processor class
class MorphologyProcessor : public FrameProcessor
{
    friend class MorphologyDialog;

public:
    MorphologyProcessor();

    // erosion processing method
    void process(cv::Mat &img, cv::Mat &out);
    int getType();
    void setType(int);
    int getSize();
    int getElem();
    void setSize(int);
    void setElem(int);

private:
    int type;
    int elem;
    int size;
};

#endif // MORPHOPROCESSOR_H
