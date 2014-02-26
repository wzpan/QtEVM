#ifndef EVMLYPRIIRDIALOG_H
#define EVMLYPRIIRDIALOG_H

#include <QDialog>
#include "EVMLyprIIRProcessor.h"

namespace Ui {
class EVMLyprIIRDialog;
}

class EVMLyprIIRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EVMLyprIIRDialog(QWidget *parent = 0,
                           EVMLyprIIRProcessor *processor = 0);
    ~EVMLyprIIRDialog();

private slots:
    void on_alphaSlider_valueChanged(int value);

    void on_lambdaSlider_valueChanged(int value);

    void on_r1Slider_valueChanged(int value);

    void on_r2Slider_valueChanged(int value);

    void on_chromSlider_valueChanged(int value);

private:
    Ui::EVMLyprIIRDialog *ui;
    EVMLyprIIRProcessor *processor;
    QString alphaStr, lambdaStr, r1Str, r2Str, chromStr;
};

#endif // EVMLYPRIIRDIALOG_H
