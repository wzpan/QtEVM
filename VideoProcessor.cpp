#include "VideoProcessor.h"

VideoProcessor::VideoProcessor(QObject *parent)
  : QObject(parent)
  , delay(-1)
  , rate(0)
  , fnumber(0)
  , length(0)
  , frameToStop(-1)
  , stop(true)
  , modify(false)
  , curPos(0)
  , currentIndex(0)
  , digits(0)
  , extension(".avi")
  , spatialType(LAPLACIAN)
  , temporalType(IIR)
  , levels(6)
  , alpha(10)
  , lambda_c(16)
  , fl(0.05)
  , fh(0.4)
  , chromAttenuation(0.1)
{
    connect(this, SIGNAL(revert()), this, SLOT(revertVideo()));
}


/** 
 * stopAtFrameNo	-	stop streaming at this frame number
 *
 * @param frame	-	frame number to stop
 */
void VideoProcessor::stopAtFrameNo(long frame)
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
void VideoProcessor::setDelay(int d)
{
    delay = d;
}

/** 
 * getNumberOfProcessedFrames	-	a count is kept of the processed frames
 *
 *
 * @return the number of processed frames
 */
long VideoProcessor::getNumberOfProcessedFrames()
{
    return fnumber;
}

/** 
 * getNumberOfPlayedFrames	-	get the current playing progress
 *
 * @return the number of played frames
 */
long VideoProcessor::getNumberOfPlayedFrames()
{
    return curPos;
}

/** 
 * getFrameSize	-	return the size of the video frame
 *
 *
 * @return the size of the video frame
 */
cv::Size VideoProcessor::getFrameSize()
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
long VideoProcessor::getFrameNumber()
{
    long f = static_cast<long>(capture.get(CV_CAP_PROP_POS_FRAMES));

    return f;
}

/** 
 * getPositionMS	-	return the position in milliseconds
 *
 * @return the position in milliseconds
 */
double VideoProcessor::getPositionMS()
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
double VideoProcessor::getFrameRate()
{
    double r = capture.get(CV_CAP_PROP_FPS);

    return r;
}

/** 
 * getLength	-	return the number of frames in video
 *
 * @return the number of frames
 */
long VideoProcessor::getLength()
{
    return length;
}


/** 
 * getLengthMS	-	return the video length in milliseconds
 *
 *
 * @return the length of length in milliseconds
 */
double VideoProcessor::getLengthMS()
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
void VideoProcessor::calculateLength()
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

void VideoProcessor::spatialFilter(const cv::Mat &src, std::vector<cv::Mat_<cv::Vec3f> > &pyramid)
{
    switch (spatialType) {
    case LAPLACIAN:     // laplacian pyramid
        buildLaplacianPyramid(src, pyramid);
        break;
    case GAUSSIAN:      // gaussian pyramid
        // TODO
        break;
    default:
        break;
    }
}

bool VideoProcessor::temporalFilter(std::vector<cv::Mat_<cv::Vec3f> > &pyramid,
                                    std::vector<cv::Mat_<cv::Vec3f> > &filtered)
{
    switch (temporalType) {
    case IIR:
        if (fnumber == 0){      // is first frame
            lowpass1 = pyramid;
            lowpass2 = pyramid;
            filtered = pyramid;
            return false;
        } else {
            for (int i=0; i<levels; i++) {
                cv::Mat temp1 = (1-fh)*lowpass1[i] + fh*pyramid[i];
                cv::Mat temp2 = (1-fl)*lowpass2[i] + fl*pyramid[i];
                lowpass1[i] = temp1;
                lowpass2[i] = temp2;
                filtered[i] = lowpass1[i] - lowpass2[i];
            }
        }
        break;
    case IDEAL:
        break;
    default:
        break;
    }
    return true;
}

/** 
 * amplify	-	ampilfy the motion
 *
 * @param filtered	- motion image
 */
void VideoProcessor::amplify(std::vector<cv::Mat_<cv::Vec3f> > &filtered)
{
    cv::Mat temp = filtered.at(0);
    int w = temp.size().width;
    int h = temp.size().height;

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
        if (l==levels || l==0) {        // ignore the highest and lowest frequency band
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

void VideoProcessor::buildLaplacianPyramid(const cv::Mat &img, std::vector<cv::Mat_<cv::Vec3f> > &lapPyr)
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

void VideoProcessor::reconImgFromPyramid(std::vector<cv::Mat_<cv::Vec3f> > &pyramid, cv::Mat &dst)
{
    switch (spatialType) {
    case LAPLACIAN:
        dst = pyramid[levels];
        for (int l=levels-1; l>=0; l--) {
            cv::Mat up;
            cv::pyrUp(dst, up, pyramid[l].size());
            dst = up + pyramid[l];
        }
        break;
    case GAUSSIAN:
        break;
    default:
        break;
    }
}

void VideoProcessor::attenuate(cv::Mat_<cv::Vec3f> &image)
{
    cv::Mat planes[3];
    cv::split(image, planes);
    planes[1] = planes[1] * chromAttenuation;
    planes[2] = planes[2] * chromAttenuation;
    cv::merge(planes, 3, image);
}

/** 
 * getCodec	-	get the codec of input video
 *
 * @param codec	-	the codec arrays
 *
 * @return the codec integer
 */
int VideoProcessor::getCodec(char codec[])
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
void VideoProcessor::getTempFile(std::string &str)
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
void VideoProcessor::getCurTempFile(std::string &str)
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
bool VideoProcessor::setInput(const std::string &fileName)
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
bool VideoProcessor::setOutput(const std::string &filename, int codec, double framerate, bool isColor)
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
bool VideoProcessor::setOutput(const std::string &filename, const std::string &ext, int numberOfDigits, int startIndex)
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
bool VideoProcessor::createTemp(double framerate, bool isColor)
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
 * setSpatialFilter	-	set the spatial filter
 *
 * @param type	-	spatial filter type. Could be:
 *					1. LAPLACIAN: laplacian pyramid
 *					2. GAUSSIAN: gaussian pyramid
 */
void VideoProcessor::setSpatialFilter(spatialFilterType type)
{
    spatialType = type;
}

/** 
 * setTemporalFilter	-	set the temporal filter
 *
 * @param type	-	temporal filter type. Could be:
 *					1. IIR: second order(IIR) filter
 *					2. IDEAL: ideal bandpass filter
 */
void VideoProcessor::setTemporalFilter(temporalFilterType type)
{
    temporalType = type;
}

/** 
 * stopIt	-	stop playing or processing
 *
 */
void VideoProcessor::stopIt()
{
    stop = true;
    emit revert();
}

/** 
 * prevFrame	-	display the prev frame of the sequence
 *
 */
void VideoProcessor::prevFrame()
{
    if(isStop())
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
void VideoProcessor::nextFrame()
{
    if(isStop())
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
bool VideoProcessor::jumpTo(long index)
{
    if (index >= length){
        return 1;
    }

    cv::Mat frame;
    bool re = capture.set(CV_CAP_PROP_POS_FRAMES, index);

    if (re && !isStop()){
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
bool VideoProcessor::jumpToMS(double pos)
{
    return capture.set(CV_CAP_PROP_POS_MSEC, pos);
}


/** 
 * close	-	close the video
 *
 */
void VideoProcessor::close()
{
    rate = 0;
    length = 0;
    modify = 0;
    capture.release();
    writer.release();
    tempWriter.release();
}


/** 
 * isStop	-	Is the processing stop
 *
 *
 * @return True if not processing/playing. False otherwise
 */
bool VideoProcessor::isStop()
{
    return stop;
}

/** 
 * isModified	-	Is the video modified?
 *
 *
 * @return True if modified. False otherwise
 */
bool VideoProcessor::isModified()
{
    return modify;
}

/** 
 * isOpened	-	Is the player opened?
 *
 *
 * @return True if opened. False otherwise 
 */
bool VideoProcessor::isOpened()
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
bool VideoProcessor::getNextFrame(cv::Mat &frame)
{
    return capture.read(frame);
}

/** 
 * writeNextFrame	-	to write the output frame
 *
 * @param frame	-	the frame to be written
 */
void VideoProcessor::writeNextFrame(cv::Mat &frame)
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
void VideoProcessor::playIt()
{
    // current frame
    cv::Mat input;

    // if no capture device has been set
    if (!isOpened())
        return;

    // is playing
    stop = false;

    // will stop at last frame
    stopAtFrameNo(length - 1);

    // update buttons
    emit updateBtn();

    while (!isStop()) {

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
    if (!isStop()){
        emit revert();
    }
}

/** 
 * pauseIt	-	pause playing
 *
 */
void VideoProcessor::pauseIt()
{
    stop = true;
    emit updateBtn();
}

/** 
 * magnify	-	eulerian magnification
 *
 */
void VideoProcessor::magnify()
{
    // create a temp file
    createTemp();

    // current frame
    cv::Mat input;
    // motion
    cv::Mat_<cv::Vec3f> motion;
    // output frame
    cv::Mat output;

    std::vector<cv::Mat_<cv::Vec3f> > pyramid;
    std::vector<cv::Mat_<cv::Vec3f> > filtered;

    // if no capture device has been set
    if (!isOpened())
        return;

    // set the modify flag to be true
    modify = true;

    // is processing
    stop = false;

    // will stop at last frame
    stopAtFrameNo(length);

    // save the current position
    long pos = curPos;
    // jump to the first frame
    jumpTo(0);

    while (!isStop()) {

        // read next frame if any
        if (!getNextFrame(input))
            break;

        // 1. convert to ntsc color space
        cv::Mat_<cv::Vec3f> s = input.clone();
        rgb2ntsc(input, s);

        // 2. spatial filtering one frame
        spatialFilter(s, pyramid);

        // 3. temporal filtering a pyramid of one frame and amplify the motion
        if (temporalFilter(pyramid, filtered)){
            amplify(filtered);
        }

        // 5. reconstruct motion image from pyramid
        reconImgFromPyramid(filtered, motion);

        // 6. attenuate I and Q channels
        attenuate(motion);

        // 7. combine the amplified motion and source image
        output = s + motion;

        // 8. convert back to rgb color space and CV_8UC3
        s = output.clone();
        ntsc2rgb(s, s);
        output = s.clone();
        double minVal, maxVal;
        minMaxLoc(output, &minVal, &maxVal); //find minimum and maximum intensities
        output.convertTo(output, CV_8UC3, 255.0/(maxVal - minVal),
                      -minVal * 255.0/(maxVal - minVal));

        // write the frame to the temp file
        tempWriter.write(output);

        // update process
        emit updateProcessProgress(floor((fnumber++) * 100.0 / length));

        // // check if we should stop
        if (frameToStop>=0 && getFrameNumber()==frameToStop)
            stopIt();
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
void VideoProcessor::writeOutput()
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
void VideoProcessor::revertVideo()
{
    // pause the video
    jumpTo(0);    
    curPos = 0;
    pauseIt();
    emit updateProgressBar();
}
