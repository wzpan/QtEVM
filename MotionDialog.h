#ifndef MOTIONDIALOG_H
#define MOTIONDIALOG_H

#include <QDialog>
#include <VideoProcessor.h>

namespace Ui {
class MotionDialog;
}

class MotionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MotionDialog(QWidget *parent = 0,
                         VideoProcessor *processor = 0);
    ~MotionDialog();

private slots:
    void on_alphaSlider_valueChanged(int value);

    void on_lambdaSlider_valueChanged(int value);

    void on_flSlider_valueChanged(int value);

    void on_fhSlider_valueChanged(int value);

    void on_chromSlider_valueChanged(int value);

private:
    Ui::MotionDialog *ui;
    VideoProcessor *processor;
    QString alphaStr, lambdaStr, flStr, fhStr, chromStr;
};

#endif // MOTIONRDIALOG_H
