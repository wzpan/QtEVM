#ifndef PARAMDIALOG_H
#define PARAMDIALOG_H

#include <QDialog>
#include <VideoProcessor.h>

namespace Ui {
class ParamDialog;
}

class ParamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParamDialog(QWidget *parent = 0,
                         VideoProcessor *processor = 0);
    ~ParamDialog();

private slots:
    void on_alphaSlider_valueChanged(int value);

    void on_lambdaSlider_valueChanged(int value);

    void on_flSlider_valueChanged(int value);

    void on_fhSlider_valueChanged(int value);

    void on_chromSlider_valueChanged(int value);

private:
    Ui::ParamDialog *ui;
    VideoProcessor *processor;
    QString alphaStr, lambdaStr, flStr, fhStr, chromStr;
};

#endif // PARAMRDIALOG_H
