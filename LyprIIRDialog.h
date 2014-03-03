#ifndef LYPRIIRDIALOG_H
#define LYPRIIRDIALOG_H

#include <QDialog>
#include "LyprIIRProcessor.h"

namespace Ui {
class LyprIIRDialog;
}

class LyprIIRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LyprIIRDialog(QWidget *parent = 0,
                           LyprIIRProcessor *processor = 0);
    ~LyprIIRDialog();

private slots:
    void on_alphaSlider_valueChanged(int value);

    void on_lambdaSlider_valueChanged(int value);

    void on_r1Slider_valueChanged(int value);

    void on_r2Slider_valueChanged(int value);

    void on_chromSlider_valueChanged(int value);

private:
    Ui::LyprIIRDialog *ui;
    LyprIIRProcessor *processor;
    QString alphaStr, lambdaStr, r1Str, r2Str, chromStr;
};

#endif // LYPRIIRDIALOG_H
