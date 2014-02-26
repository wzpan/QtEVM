#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rateLabel = new QLabel;         // Frame rate

    // remind users to open a video
    inputTip = "Please open a video.";
    ui->videoLabel->setText(inputTip);

    // Status bar
    rateLabel->setText("");
    ui->statusBar->addPermanentWidget(rateLabel);

    // progress dialog
    progressDialog =  0;

    // Laplacian IIR amplify
    lyprIIRDialog = 0;
    lyprIIRProcessor = 0;

    updateStatus(false);

    video = new VideoProcessor;

    connect(video, SIGNAL(showFrame(cv::Mat)), this, SLOT(showFrame(cv::Mat)));
    connect(video, SIGNAL(sleep(int)), this, SLOT(sleep(int)));
    connect(video, SIGNAL(revert()), this, SLOT(revert()));
    connect(video, SIGNAL(updateBtn()), this, SLOT(updateBtn()));
    connect(video, SIGNAL(updateProgressBar()), this, SLOT(updateProgressBar()));
    connect(video, SIGNAL(updateProcessProgress(int)), this, SLOT(updateProcessProgress(int)));
    connect(video, SIGNAL(closeProgressDialog()), this, SLOT(closeProgressDialog()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * updateEnable	-	Update the enabled property of play menu and filter menu
 *
 * @param vi	-	the enable variable
 */
void MainWindow::updateStatus(bool vi)
{
    for (int i = 0; i < ui->menuPlay->actions().count(); ++i){
        ui->menuPlay->actions().at(i)->setEnabled(vi);
    }
    for (int i = 0; i < ui->menuProcessor->actions().count(); ++i){
        ui->menuProcessor->actions().at(i)->setEnabled(vi);
    }
    ui->actionClose->setEnabled(vi);
    ui->actionSave_as->setEnabled(vi);
    ui->progressSlider->setEnabled(vi);
    ui->btnLast->setEnabled(vi);;
    ui->btnNext->setEnabled(vi);
    ui->btnPlay->setEnabled(vi);
    ui->btnStop->setEnabled(vi);
    ui->btnPause->setEnabled(vi);


    if(!vi){
        ui->progressSlider->setValue(0);
        rateLabel->setText("");
    }
}

void MainWindow::updateTimeLabel()
{
    QString curPos = QDateTime::fromMSecsSinceEpoch(
                video->getPositionMS()).toString("hh:mm::ss");
    QString length = QDateTime::fromMSecsSinceEpoch(
                video->getLengthMS()).toString("hh:mm::ss");
    ui->timeLabel->setText(tr("<span style=' color:#ff0000;'>"
                              "%1</span> / %2").arg(curPos, length));
}

/** 
 * clean	-	clean all temp files
 *
 */
void MainWindow::clean()
{
    QString path = ".";
    QDir dir(path);

    std::string dirFile;

    while (true){
        video->getTempFile(dirFile);
        if (dirFile == "")
            break;
        else
            dir.remove(QString::fromStdString(dirFile));
    }

}


/** 
 * closeEvent	-	Do a file save check before close the application
 *
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
        clean();
    } else {
        event->ignore();
    }
}

/** 
 * maybeSave	-	some thing to do before the next operation
 *
 * @return true if all preparation has done; false if the user cancel the operation.
 */
bool MainWindow::maybeSave()
{
    if (video->isModified()){
        // Create an warning dialog
        QMessageBox box;
        box.setWindowTitle(tr("Warning"));
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("The current file %1 has been changed. Save?").arg(curFile));

        QPushButton *yesBtn = box.addButton(tr("Yes(&Y)"),
                                            QMessageBox::YesRole);
        box.addButton(tr("No(&N)"), QMessageBox::NoRole);
        QPushButton *cancelBut = box.addButton(tr("Cancel(&C)"),
                                               QMessageBox::RejectRole);

        box.exec();
        if (box.clickedButton() == yesBtn)
            return saveAs();
        if (box.clickedButton() == cancelBut)
            return false;
    }
    return true;
}

/** 
 * play	-	play the video
 *
 */
void MainWindow::play()
{
    video->setDelay(1000 / video->getFrameRate());
    video->playIt();
}


/** 
 * saveAs	-	Save the file as...
 *
 *
 * @return true if the file is successfully saved to the
 * 			user defined location
 */
bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save as"),
                                                    curFile);

    if (fileName.isEmpty()) return false;
    return saveFile(fileName);
}


/** 
 * saveFile	-	Save the file to a specified location
 *
 * @param fileName	-	the target location
 *
 * @return true if the file is successfully saved
 */
bool MainWindow::saveFile(const QString &fileName)
{
    if (QFileInfo(fileName).suffix() == ""){
        video->setOutput(fileName.toStdString(), ".avi");
    } else {
        video->setOutput(QFileInfo(fileName).fileName().toStdString());
    }

    // change the cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // save all the contents to file
    video->writeOutput();

    // restore the cursor
    QApplication::restoreOverrideCursor();

    // set the current file location
    curFile = QFileInfo(fileName).canonicalPath();
    setWindowTitle(curFile);

    return true;
}


/** 
 * loadFile	-	Open and load a video
 *
 * @param fileName	-	file location
 *
 * @return true if the file is successfully loaded
 */
bool MainWindow::LoadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("VideoPlayer"),
                             tr("Unable to load file %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }

    // change the cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // input file
    if (!video->setInput(fileName.toStdString())){
        QMessageBox::warning(this, tr("VideoPlayer"),
                             tr("Unable to load file %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }

    // restore the cursor
    QApplication::restoreOverrideCursor();

    // update the frame rate
    rateLabel->setText(tr("Frame rate: %1").arg(video->getFrameRate()));

    // update the time label
    updateTimeLabel();

    // set the current file location
    curFile = QFileInfo(fileName).canonicalPath();
    setWindowTitle(curFile);
    return true;
}

void MainWindow::process()
{
    // change the cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // run the process
    video->runProcess();

    // restore the cursor
    QApplication::restoreOverrideCursor();
}


/** 
 * showImage	-	show a image
 *
 * @param image	-	the image to be showed
 *
 * Mainly used for display a single frame
 */
void MainWindow::showFrame(cv::Mat frame)
{
    cv::Mat tmp;
    cvtColor(frame, tmp, CV_BGR2RGB);
    QImage img = QImage((const unsigned char*)(tmp.data),
                        tmp.cols, tmp.rows, QImage::Format_RGB888);

    ui->videoLabel->setPixmap(QPixmap::fromImage(img));
    ui->videoLabel->repaint();
}


/** 
 * revert	-	do something when revert playing
 *
 */
void MainWindow::revert()
{
    updateBtn();
}

/** 
 * sleep	-	pause the video for several msecs
 *
 * @param msecs 
 */
void MainWindow::sleep(int msecs)
{
    helper->sleep(msecs);
}

/** 
 * updateBtn	-	update the button
 *
 */
void MainWindow::updateBtn()
{
    bool isPlay = video->isPlay();
    ui->actionPause->setVisible(isPlay);
    ui->btnPause->setVisible(isPlay);
    ui->actionPlay->setVisible(!isPlay);
    ui->btnPlay->setVisible(!isPlay);
}

/** 
 * updateProgressBar	-	update the progress bar
 *
 */
void MainWindow::updateProgressBar()
{
    // update the progress bar
    ui->progressSlider->setValue(video->getNumberOfPlayedFrames()
                                 * ui->progressSlider->maximum()
                                 / video->getLength() * 1.0);

    // update the time label
    updateTimeLabel();
}


/**
 * updateProgressBar	-	update the process progress dialog
 *
 */
void MainWindow::updateProcessProgress(int value)
{
    if(!progressDialog){
        progressDialog = new QProgressDialog(this);
        progressDialog->setLabelText("Processing, please wait...");
        progressDialog->setRange(0, 100);
        progressDialog->setModal(true);
    }
    progressDialog->setValue(value);
    progressDialog->setCancelButton(0);
    qApp->processEvents();

}

/**
 * about	-	Display about info
 *
 */
void MainWindow::about()
{
    QMessageBox::about(this, tr("About VideoPlayer"),
                       tr("<h2>VideoPlayer 1.0</h2>"
                          "<p>Copyright &copy; 2013 Software Inc.</p>"
                          "<p>VideoPlayer is a small application to "
                          "play and process videos.</p>"));
}

// Open video file
void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"),
                                                    ".",
                                                    tr("Video Files (*.avi *.mov *.mpeg *.mp4)"));
    if(!fileName.isEmpty()) {
        if(LoadFile(fileName)){
            updateStatus(true);
            updateBtn();
        }
    }
}

// Quit
void MainWindow::on_actionQuit_triggered()
{
    // Execute the close operation before quit.
    on_actionClose_triggered();

    qApp->quit();
}

// Close video
void MainWindow::on_actionClose_triggered()
{
    if (maybeSave()) {
        updateStatus(false);
        ui->videoLabel->setText(inputTip);
        video->close();
        clean();
        ui->timeLabel->setText("");
        rateLabel->setText("");
    }
}

// Save As operation
void MainWindow::on_actionSave_as_triggered()
{
    saveAs();
}

// About
void MainWindow::on_actionAbout_triggered()
{
    about();
}

// About Qt
void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

// stop action
void MainWindow::on_actionStop_S_triggered()
{
    video->stopIt();
}

// stop button action
void MainWindow::on_btnStop_clicked()
{
    video->stopIt();
}

// play button action
void MainWindow::on_btnPlay_clicked()
{
    play();
}

// play action
void MainWindow::on_actionPlay_triggered()
{
    play();
}

// pause action
void MainWindow::on_actionPause_triggered()
{
    video->pauseIt();
}

// pause button
void MainWindow::on_btnPause_clicked()
{
    video->pauseIt();
}

// next frame
void MainWindow::on_btnNext_clicked()
{
    video->nextFrame();
}

// prev frame
void MainWindow::on_btnLast_clicked()
{
    video->prevFrame();
}

// slider moved
void MainWindow::on_progressSlider_sliderMoved(int position)
{
    long pos = position * video->getLength() /
            ui->progressSlider->maximum();
    video->jumpTo(pos);

    updateTimeLabel();
}

void MainWindow::closeProgressDialog()
{
    progressDialog->close();
    progressDialog = 0;
}

// Clean all the temp files
void MainWindow::on_actionClean_Temp_Files_triggered()
{
    QString path = ".";
    QDir dir(path);

    // set filter
    dir.setNameFilters(QStringList() << "temp*.avi");
    dir.setFilter(QDir::Files);
    std::string temp;

    // delete all the temp files
    foreach(QString dirFile, dir.entryList()){
        video->getCurTempFile(temp);
        if (dirFile != QString::fromStdString(temp))
            dir.remove(dirFile);
    }
}

void MainWindow::on_actionLapliacian_IIR_triggered()
{
    if (!lyprIIRProcessor)
        lyprIIRProcessor = new EVMLyprIIRProcessor();
    lyprIIRProcessor->reset();

    // show laplacian IIR dialog
    if (!lyprIIRDialog)
        lyprIIRDialog = new EVMLyprIIRDialog(this, lyprIIRProcessor);

    lyprIIRDialog->show();
    lyprIIRDialog->raise();
    lyprIIRDialog->activateWindow();

    if (lyprIIRDialog->exec() == QDialog::Accepted) {
        // set laplacian IIR motion magnification processor as the current frame processor
        video->setFrameProcessor(lyprIIRProcessor);
        // do process
        process();
    }
}
