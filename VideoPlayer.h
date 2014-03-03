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
#include "FrameProcessor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dbg.h>

class VideoPlayer : public QObject {

    Q_OBJECT

public:

    explicit VideoPlayer(QObject *parent = 0);

    // Is the player playing?
    bool isPlay();

    // Is the video modified?
    bool isModified();

    // Is the player opened?
    bool isOpened();

    // stop streaming at this frame number
    void stopAtFrameNo(long frame);

    // set a delay between each frame
    // 0 means wait at each frame
    // negative means no delay
    void setDelay(int d);

    // a count is kept of the processed frames
    long getNumberOfProcessedFrames();

    // get the current playing progress
    long getNumberOfPlayedFrames();

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

    // set the callback function that will be called for each frame
    void setFrameProcessor(void (*frameProcessingCallback)(cv::Mat&, cv::Mat&));

    // set the instance of the class that implements the FrameProcessor interface
    void setFrameProcessor(FrameProcessor* frameProcessorPtr);

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

    // process the frames of the sequence
    void processFrame();

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
    void updateProcessProgress(int percent);
    void closeProgressDialog();

private:    

    // the OpenCV video capture object
    cv::VideoCapture capture;

    // the callback function to be called
    // for the processing of each frame
    void (*process)(cv::Mat&, cv::Mat&);
    // the pointer to the class implementing
    // the FrameProcessor interface
    FrameProcessor *frameProcessor;
    // delay between each frame processing
    int delay;
    // video frame rate
    double rate;
    // number of processed frames
    long fnumber;
    // total number of frames
    long length;
    // stop at this frame number
    long frameToStop;
    // to stop the player
    bool play;
    // is the video modified
    bool modify;
    // the current playing pos
    long curPos;

    // the OpenCV video writer object
    cv::VideoWriter writer;
    cv::VideoWriter tempWriter;

    // output filename
    std::string outputFile;
    // temp filename
    std::string tempFile;
    // all temp files queue
    std::vector<std::string> tempFileList;

    // current index for output images
    int currentIndex;
    // number of digits in output image filename
    int digits;
    // extension of output images
    std::string extension;

    // get the next frame if any
    bool getNextFrame(cv::Mat& frame);

    // to write the output frame
    void writeNextFrame(cv::Mat& frame);

    // set the temp video file
    // by default the same parameters to the input video
    bool createTemp(double framerate=0.0, bool isColor=true);

    // recalculate the number of frames in video
    // normally doesn't need it unless getLength()
    // can't return a valid value
    void calculateLength();

};

#endif // VIDEOPROCESSOR_H
