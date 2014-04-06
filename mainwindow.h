// Yet anther C++ implementation of EVM, based on OpenCV and Qt. 
// Copyright (C) 2014  Joseph Pan <cs.wzpan@gmail.com>
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301 USA
// 

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDateTime>
#include <QTime>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QProgressDialog>
#include <QLabel>
#include <queue>
#include "VideoProcessor.h"
#include "MagnifyDialog.h"
#include "WindowHelper.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class VideoProcessor;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

    // File functions
	bool maybeSave();   // whether needs save
    bool save();        // save
    void play();        // save
    bool saveAs();      // save as
    bool saveFile(const QString &fileName);     // save file
    bool LoadFile(const QString &fileName);    // load file

    // Help functions
    void about();
    void aboutQt();

private slots:
    void on_actionOpen_triggered();

    void on_actionQuit_triggered();

    void on_actionClose_triggered();

    void on_actionSave_as_triggered();

    void on_actionAbout_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionStop_S_triggered();

    void on_btnPlay_clicked();

    void on_btnStop_clicked();

    void on_actionPlay_triggered();

    void on_actionPause_triggered();

    void on_btnPause_clicked();

    void on_btnNext_clicked();

    void on_btnLast_clicked();  

    void on_actionClean_Temp_Files_triggered();

    void showFrame(cv::Mat frame);      // show a frame

    void revert();                      // revert playing

    void sleep(int msecs);              // sleep for a while

    void updateBtn();                   // update button status

    void updateProgressBar();           // update progress bar

    void updateProcessProgress(const std::string &message, int value);  // update process progress

    void on_progressSlider_sliderMoved(int position);

    void closeProgressDialog();

    void on_motion_triggered();

    void on_color_triggered();

protected:
    void closeEvent(QCloseEvent *);
    
private:
    Ui::MainWindow *ui;

    // Process progress
    QProgressDialog *progressDialog;

    // Motion Magnification dialog
    MagnifyDialog *magnifyDialog;

    void updateStatus(bool vi);
    void updateTimeLabel();

    // tips when no image is opened
    QString inputTip;

    // frame label
    QLabel *rateLabel;

    // current file's location
    QString curFile;

    // video processor instance
    VideoProcessor *video;

    // a window helper with some useful
    // functions e.g. sleep
    WindowHelper *helper;

    // for cleaning temp files
    void clean();
};

#endif // MAINWINDOW_H
