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
#include "EVMLyprIIRDialog.h"
#include "EVMLyprIIRProcessor.h"
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
    void process();     // process

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

    void updateProcessProgress(int value);  // update process progress

    void on_progressSlider_sliderMoved(int position);

    void closeProgressDialog();

    void on_actionLapliacian_IIR_triggered();

protected:
    void closeEvent(QCloseEvent *);
    
private:
    Ui::MainWindow *ui;

    // Process progress
    QProgressDialog *progressDialog;

    // Laplacian IIR amplify dialog
    EVMLyprIIRDialog *lyprIIRDialog;
    // Laplacian IIR amplify processor
    EVMLyprIIRProcessor *lyprIIRProcessor;

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
