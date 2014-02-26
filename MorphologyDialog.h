#ifndef MORPHODIALOG_H
#define MORPHODIALOG_H

#include <QDialog>
#include "MorphologyProcessor.h"

namespace Ui {
class MorphologyDialog;
}

class MorphologyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MorphologyDialog(QWidget *parent = 0,
                           MorphologyProcessor *processor = 0);
    ~MorphologyDialog();
    int getSize();
    int getElem();
    void setSize(int);
    void setElem(int);
    void setParam(int, int);

private slots:
    void on_comboElement_currentIndexChanged(int index);
    void on_sizeSlider_valueChanged(int value);
    void on_comboType_currentIndexChanged(int index);

signals:
    void process();

private:
    Ui::MorphologyDialog *ui;
    MorphologyProcessor *processor;
};

#endif // MORPHODIALOG_H
