#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <QObject>
#include <QDateTime>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SpatialFilter.h"
#include "TemporalFilter.h"
#include "Utils.h"

enum spatialFilterType {LAPLACIAN, GAUSSIAN};
enum temporalFilterType {IIR, IDEAL};

class VideoProcessor : public QObject {

    Q_OBJECT

    friend class ParamDialog;

public:

    explicit VideoProcessor(QObject *parent = 0);

    // Is the player playing?
    bool isStop();

    // Is the video modified?
    bool isModified();

    // Is the player opened?
    bool isOpened();

    // set a delay between each frame
    // 0 means wait at each frame
    // negative means no delay
    void setDelay(int d);

    // a count is kept of the processed frames
    long getNumberOfProcessedFrames();

    // get the current playing progress
    long getNumberOfPlayedFrames();

    // get the video capture
    cv::VideoCapture getCapture();

    // return the size of the video frame
    cv::Size getFrameSize();

    // return the frame number of the next frame
    long getFrameNumber();

    // return the position in milliseconds
    double getPositionMS();

    // return the frame rate
    double getFrameRate();

    // return the number of frames in video
    long getLength();

    // return the video length in milliseconds
    double getLengthMS();

    // get the codec of input video
    int getCodec(char codec[4]);

    // get temp file lists
    void getTempFile(std::string &);

    // get current temp file
    void getCurTempFile(std::string &);

    // go to this position expressed in fraction of total film length
    bool setRelativePosition(double pos);

    // set the name of the video file
    bool setInput(const std::string &fileName);

    // set the output video file
    // by default the same parameters than input video will be used
    bool setOutput(const std::string &filename, int codec=0, double framerate=0.0, bool isColor=true);

    // set the output as a series of image files
    // extension must be ".jpg", ".bmp" ...
    bool setOutput(const std::string &filename, // filename prefix
                   const std::string &ext, // image file extension
                   int numberOfDigits=3,   // number of digits
                   int startIndex=0);       // start index

    // set spatial filter
    void setSpatialFilter(spatialFilterType type);

    // set temporal filter
    void setTemporalFilter(temporalFilterType type);

    // play the frames of the sequence
    void playIt();

    // pause the frames of the sequence
    void pauseIt();

    // Stop playing
    void stopIt();

    // display the prev frame of the sequence
    void prevFrame();

    // display the next frame of the sequence
    void nextFrame();

    // Jump to a position
    bool jumpTo(long index);

    // Jump to a position in milliseconds
    bool jumpToMS(double pos);

    // close the video
    void close();

    // motion magnification
    void motionMagnify();

    // color magnification
    void colorMagnify();

    // write the processed result
    void writeOutput();

private slots:
    void revertVideo();

signals:
    void showFrame(cv::Mat frame);
    void revert();
    void sleep(int msecs);
    void updateBtn();
    void updateProgressBar();
    void reload(const std::string &);
    void updateProcessProgress(const std::string &message, int percent);
    void closeProgressDialog();

private:    

    // the OpenCV video capture object
    cv::VideoCapture capture;

    // delay between each frame processing
    int delay;
    // video frame rate
    double rate;
    // number of processed frames
    long fnumber;
    // total number of frames
    long length;
    // to stop the player
    bool stop;
    // is the video modified
    bool modify;
    // the current playing pos
    long curPos;
    // current index for output images
    int curIndex;
    // current level of pyramid
    int curLevel;
    // number of digits in output image filename
    int digits;    
    // extension of output images
    std::string extension;
    // spatial filter type
    spatialFilterType spatialType;
    // temporal filter type
    temporalFilterType temporalType;
    // level numbers of image pyramid
    int levels;
    // amplification factor
    float alpha;
    // cut-off wave length
    float lambda_c;    
    // low cut-off
    float fl;
    // high cut-off
    float fh;
    // chromAttenuation
    float chromAttenuation;
    // delta
    float delta;
    // extraggon factor
    float exaggeration_factor;
    // lambda
    float lambda;
    // the OpenCV video writer object
    cv::VideoWriter writer;
    cv::VideoWriter tempWriter;

    // output filename
    std::string outputFile;
    // temp filename
    std::string tempFile;
    // all temp files queue
    std::vector<std::string> tempFileList;

    // low pass filters for IIR
    std::vector<cv::Mat_<cv::Vec3f> > lowpass1;
    std::vector<cv::Mat_<cv::Vec3f> > lowpass2;

    // recalculate the number of frames in video
    // normally doesn't need it unless getLength()
    // can't return a valid value
    void calculateLength();

    // get the next frame if any
    bool getNextFrame(cv::Mat& frame);

    // to write the output frame
    void writeNextFrame(cv::Mat& frame);

    // set the temp video file
    // by default the same parameters to the input video
    bool createTemp(double framerate=0.0, bool isColor=true);

    // spatial filtering
    bool spatialFilter(const cv::Mat &src, std::vector<cv::Mat_<cv::Vec3f> > &pyramid);

    // temporal filtering
    void temporalFilter(const cv::Mat_<cv::Vec3f> &src,
                        cv::Mat_<cv::Vec3f> &dst);

    // temporal IIR filtering
    void temporalIIRFilter(const cv::Mat_<cv::Vec3f> &src,
                        cv::Mat_<cv::Vec3f> &dst);

    // temporal ideal bandpass filtering
    void temporalIdealFilter(const cv::Mat_<cv::Vec3f> &src,
                             cv::Mat_<cv::Vec3f> &dst);

    // amplify motion
    void amplify(const cv::Mat_<cv::Vec3f> &src, cv::Mat_<cv::Vec3f> &dst);

    // attenuate I, Q channels
    void attenuate(cv::Mat &src, cv::Mat &dst);

    // concat images into a large Mat
    void concat(const std::vector<cv::Mat_<cv::Vec3f> > &frames, cv::Mat_<cv::Vec3f> &dst);

    // de-concat the concatnate image into frames
    void deConcat(const cv::Mat_<cv::Vec3f> &src, const cv::Size &frameSize, std::vector<cv::Mat_<cv::Vec3f> > &frames);
};

#endif // VIDEOPROCESSOR_H
