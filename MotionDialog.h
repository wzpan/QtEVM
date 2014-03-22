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
