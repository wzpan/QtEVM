#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(QObject *parent) : QObject(parent)
{
    delay = -1;
    fnumber = 0;
    play = false;
    digits = 0;
    frameToStop = -1;
    process = 0;
    frameProcessor = 0;
    modify = false;
    length = 0;
    curPos = 0;

    connect(this, SIGNAL(revert()), this, SLOT(revertVideo()));
}


/** 
 * stopAtFrameNo	-	stop streaming at this frame number
 *
 * @param frame	-	frame number to stop
 */
void VideoPlayer::stopAtFrameNo(long frame)
{
    frameToStop = frame;
}


/** 
 * setDelay	-	 set a delay between each frame
 *
 * 0 means wait at each frame, 
 * negative means no delay
 * @param d	-	delay param
 */
void VideoPlayer::setDelay(int d)
{
    delay = d;
}

/** 
 * getNumberOfProcessedFrames	-	a count is kept of the processed frames
 *
 *
 * @return the number of processed frames
 */
long VideoPlayer::getNumberOfProcessedFrames()
{
    return fnumber;
}

/** 
 * getNumberOfPlayedFrames	-	get the current playing progress
 *
 * @return the number of played frames
 */
long VideoPlayer::getNumberOfPlayedFrames()
{
    return curPos;
}

/** 
 * getFrameSize	-	return the size of the video frame
 *
 *
 * @return the size of the video frame
 */
cv::Size VideoPlayer::getFrameSize()
{
    int w = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    return cv::Size(w,h);
}

/** 
 * getFrameNumber	-	return the frame number of the next frame
 *
 *
 * @return the frame number of the next frame
 */
long VideoPlayer::getFrameNumber()
{
    long f = static_cast<long>(capture.get(CV_CAP_PROP_POS_FRAMES));

    return f;
}

/** 
 * getPositionMS	-	return the position in milliseconds
 *
 * @return the position in milliseconds
 */
double VideoPlayer::getPositionMS()
{
    double t = capture.get(CV_CAP_PROP_POS_MSEC);

    return t;
}

/** 
 * getFrameRate	-	return the frame rate
 *
 *
 * @return the frame rate
 */
double VideoPlayer::getFrameRate()
{
    double r = capture.get(CV_CAP_PROP_FPS);

    return r;
}

/** 
 * getLength	-	return the number of frames in video
 *
 * @return the number of frames
 */
long VideoPlayer::getLength()
{
    return length;
}


/** 
 * getLengthMS	-	return the video length in milliseconds
 *
 *
 * @return the length of length in milliseconds
 */
double VideoPlayer::getLengthMS()
{
    double l = 1000.0 * length / rate;
    return l;
}

/** 
 * calculateLength	-	recalculate the number of frames in video
 * 
 * normally doesn't need it unless getLength()
 * can't return a valid value
 */
void VideoPlayer::calculateLength()
{
    long l = 0;
    cv::Mat img;
    cv::VideoCapture tempCapture(tempFile);
    while(tempCapture.read(img)){
        ++l;
    }
    length = l;
    tempCapture.release();
}

/** 
 * getCodec	-	get the codec of input video
 *
 * @param codec	-	the codec arrays
 *
 * @return the codec integer
 */
int VideoPlayer::getCodec(char codec[])
{
    union {
        int value;
        char code[4]; } returned;

    returned.value = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));

    codec[0] = returned.code[0];
    codec[1] = returned.code[1];
    codec[2] = returned.code[2];
    codec[3] = returned.code[3];

    return returned.value;
}


/** 
 * getTempFile	-	temp file lists
 *
 * @param str	-	the reference of the output string
 */
void VideoPlayer::getTempFile(std::string &str)
{
    if (!tempFileList.empty()){
        str = tempFileList.back();
        tempFileList.pop_back();
    } else {
        str = "";
    }
}

/** 
 * getCurTempFile	-	get current temp file
 *
 * @param str	-	the reference of the output string
 */
void VideoPlayer::getCurTempFile(std::string &str)
{
    str = tempFile;
}

/** 
 * setInput	-	set the name of the expected video file
 *
 * @param fileName	-	the name of the video file
 *
 * @return True if success. False otherwise
 */
bool VideoPlayer::setInput(const std::string &fileName)
{
    fnumber = 0;
    tempFile = fileName;

    // In case a resource was already
    // associated with the VideoCapture instance
    if (isOpened()){
        capture.release();
    }

    // Open the video file
    if(capture.open(fileName)){
        cv::Mat input;
        getNextFrame(input);
        length = capture.get(CV_CAP_PROP_FRAME_COUNT);
        rate = getFrameRate();
        emit showFrame(input);
        emit updateBtn();
        return true;
    } else {
        return false;
    }
}

/** 
 * setOutput	-	set the output video file
 *
 * by default the same parameters than input video will be used
 *
 * @param filename	-	filename prefix
 * @param codec		-	the codec
 * @param framerate -	frame rate
 * @param isColor	-	is the video colorful
 *
 * @return True if successful. False otherwise
 */
bool VideoPlayer::setOutput(const std::string &filename, int codec, double framerate, bool isColor)
{
    outputFile = filename;
    extension.clear();

    if (framerate==0.0)
        framerate = getFrameRate(); // same as input

    char c[4];
    // use same codec as input
    if (codec==0) {
        codec = getCodec(c);
    }

    // Open output video
    return writer.open(outputFile, // filename
                       codec, // codec to be used
                       framerate,      // frame rate of the video
                       getFrameSize(), // frame size
                       isColor);       // color video?
}

/** 
 * set the output as a series of image files
 *
 * extension must be ".jpg", ".bmp" ...
 *
 * @param filename	-	filename prefix
 * @param ext		-	image file extension
 * @param numberOfDigits	-	number of digits
 * @param startIndex	-	start index
 *
 * @return True if successful. False otherwise
 */
bool VideoPlayer::setOutput(const std::string &filename, const std::string &ext, int numberOfDigits, int startIndex)
{
    // number of digits must be positive
    if (numberOfDigits<0)
        return false;

    // filenames and their common extension
    outputFile = filename;
    extension = ext;

    // number of digits in the file numbering scheme
    digits = numberOfDigits;
    // start numbering at this index
    currentIndex = startIndex;

    return true;
}

/** 
 * setTemp	-	set the temp video file
 *
 * by default the same parameters to the input video
 *
 * @param codec	-	video codec
 * @param framerate	-	frame rate
 * @param isColor	-	is the video colorful
 *
 * @return True if successful. False otherwise
 */
bool VideoPlayer::createTemp(double framerate, bool isColor)
{
    std::stringstream ss;
    ss << "temp_" << QDateTime::currentDateTime().toTime_t() << ".avi";
    tempFile = ss.str();

    tempFileList.push_back(tempFile);

    if (framerate==0.0)
        framerate = getFrameRate(); // same as input

    // Open output video
    return tempWriter.open(tempFile, // filename
                       CV_FOURCC('M', 'J', 'P', 'G'), // codec to be used
                       framerate,      // frame rate of the video
                       getFrameSize(), // frame size
                       isColor);       // color video?
}

/** 
 * setFrameProcessor	-	set the callback function that will be called for each frame
 *
 * @param frameProcessingCallback	-	callback function for processing
 */
void VideoPlayer::setFrameProcessor(void (*frameProcessingCallback)(cv::Mat&, cv::Mat&))
{
    // invalidate frame processor class instance
    frameProcessor = 0;
    // this is the frame processor function that will be called
    process = frameProcessingCallback;
}

/** 
 * setFrameProcessor	-	set the instance of the class that implements the FrameProcessor interface
 *
 * @param frameProcessorPtr -	the instance of the class that implements the FrameProcessor interface
 */
void VideoPlayer::setFrameProcessor(FrameProcessor *frameProcessorPtr)
{
    // invalidate callback function
    process = 0;
    // this is the frame processor instance that will be called
    frameProcessor = frameProcessorPtr;
}

/** 
 * stopIt	-	stop playing
 *
 */
void VideoPlayer::stopIt()
{
    play = false;
    emit revert();
}

/** 
 * prevFrame	-	display the prev frame of the sequence
 *
 */
void VideoPlayer::prevFrame()
{
    if(isPlay())
        pauseIt();
    if (curPos >= 0){
        curPos -= 1;
        jumpTo(curPos);
    }
    emit updateProgressBar();
}

/** 
 * nextFrame	-	display the next frame of the sequence
 *
 */
void VideoPlayer::nextFrame()
{
    if(isPlay())
        pauseIt();
    curPos += 1;
    if (curPos <= length){
        curPos += 1;
        jumpTo(curPos);
    }
    emit updateProgressBar();
}

/** 
 * jumpTo	-	Jump to a position
 *
 * @param index	-	frame index
 *
 * @return True if success. False otherwise
 */
bool VideoPlayer::jumpTo(long index)
{
    if (index >= length){
        return 1;
    }

    cv::Mat frame;
    bool re = capture.set(CV_CAP_PROP_POS_FRAMES, index);

    if (re && !isPlay()){
        capture.read(frame);
        emit showFrame(frame);
    }

    return re;
}


/** 
 * jumpToMS	-	jump to a position at a time
 *
 * @param pos	-	time
 *
 * @return True if success. False otherwise
 *
 */
bool VideoPlayer::jumpToMS(double pos)
{
    return capture.set(CV_CAP_PROP_POS_MSEC, pos);
}


/** 
 * close	-	close the video
 *
 */
void VideoPlayer::close()
{
    rate = 0;
    length = 0;
    modify = 0;
    capture.release();
    writer.release();
    tempWriter.release();
}

/** 
 * isPlay	-	Is the player playing?
 *
 *
 * @return True if playing. False otherwise
 */
bool VideoPlayer::isPlay()
{
    return play;
}

/** 
 * isModified	-	Is the video modified?
 *
 *
 * @return True if modified. False otherwise
 */
bool VideoPlayer::isModified()
{
    return modify;
}

/** 
 * isOpened	-	Is the player opened?
 *
 *
 * @return True if opened. False otherwise 
 */
bool VideoPlayer::isOpened()
{
    return capture.isOpened();
}

/** 
 * getNextFrame	-	get the next frame if any
 *
 * @param frame	-	the expected frame
 *
 * @return True if success. False otherwise
 */
bool VideoPlayer::getNextFrame(cv::Mat &frame)
{
    return capture.read(frame);
}

/** 
 * writeNextFrame	-	to write the output frame
 *
 * @param frame	-	the frame to be written
 */
void VideoPlayer::writeNextFrame(cv::Mat &frame)
{
    if (extension.length()) { // then we write images

        std::stringstream ss;
        ss << outputFile << std::setfill('0') << std::setw(digits) << currentIndex++ << extension;
        cv::imwrite(ss.str(),frame);

    } else { // then write video file

        writer.write(frame);
    }
}

/** 
 * playIt	-	play the frames of the sequence
 *
 */
void VideoPlayer::playIt()
{
    // current frame
    cv::Mat input;

    // if no capture device has been set
    if (!isOpened())
        return;

    // set the play flag to be true
    play = true;

    // update buttons
    emit updateBtn();

    while (isPlay()) {

        // read next frame if any
        if (!getNextFrame(input))
            break;

        curPos = capture.get(CV_CAP_PROP_POS_FRAMES);

        // display input frame
        emit showFrame(input);

        // update the progress bar
        emit updateProgressBar();

        // introduce a delay
        emit sleep(delay);

        // check if we should stop
        if (frameToStop>=0 && getFrameNumber()==frameToStop)
            stopIt();
    }
    if (isPlay()){
        emit revert();
    }
}

/** 
 * pauseIt	-	pause playing
 *
 */
void VideoPlayer::pauseIt()
{
    play = false;
    emit updateBtn();
}

/** 
 * processFrame	-	process the frames of the sequence
 *
 */
void VideoPlayer::processFrame()
{
    // create a temp file
    createTemp();

    // current frame
    cv::Mat input;
    // output frame
    cv::Mat output;

    // if no capture device has been set
    if (!isOpened())
        return;

    // set the modify flag to be true
    modify = true;

    // save the current position
    long pos = curPos;
    // jump to the first frame
    jumpTo(0);

    while (getNextFrame(input)) {

        // process the frame
        if (process)
            process(input, output);
        else if (frameProcessor)
            frameProcessor->process(input, output);

        fnumber++;

        // update process
        emit updateProcessProgress(floor(fnumber * 100.0 / length));

        // write the frame to the temp file
        tempWriter.write(output);
    }

    emit closeProgressDialog();

    // release the temp writer
    tempWriter.release();

    // change the video to the processed video 
    setInput(tempFile);

    // jump back to the original position
    jumpTo(pos);
}


/** 
 * writeOutput	-	write the processed result
 *
 */
void VideoPlayer::writeOutput()
{
    cv::Mat input;

    // if no capture device has been set
    if (!isOpened() || !writer.isOpened())
        return;

    // save the current position
    long pos = curPos;
    
    // jump to the first frame
    jumpTo(0);

    while (getNextFrame(input)) {

        // write output sequence
        if (outputFile.length()!=0)
            writeNextFrame(input);
    }

    // set the modify flag to false
    modify = false;

    // release the writer
    writer.release();

    // jump back to the original position
    jumpTo(pos);
}

/** 
 * revertVideo	-	revert playing
 *
 */
void VideoPlayer::revertVideo()
{
    // pause the video
    jumpTo(0);    
    curPos = 0;
    pauseIt();
    emit updateProgressBar();
}
