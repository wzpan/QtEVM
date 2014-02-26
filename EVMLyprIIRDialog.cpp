#include "EVMLyprIIRDialog.h"
#include <string.h>
#include "ui_EVMLyprIIRDialog.h"

EVMLyprIIRDialog::EVMLyprIIRDialog(QWidget *parent,
                                   EVMLyprIIRProcessor *processor) :
    QDialog(parent),
    ui(new Ui::EVMLyprIIRDialog)
{
    this->processor = processor;
    ui->setupUi(this);

    alphaStr = "Amplification:";
    lambdaStr = "Cut-off Wavelength:";
    r1Str = "r1 (High cut-off?):";
    r2Str = "r2 (Low cut-off?):";
    chromStr = "ChromAttenuation:";

    ui->alphaSlider->setValue(processor->alpha);
    ui->lambdaSlider->setValue(processor->lambda_c);
    ui->r1Slider->setValue(processor->r1 * 100);
    ui->r2Slider->setValue(processor->r2 * 100);
    ui->chromSlider->setValue(processor->chromAttenuation * 10);

    std::stringstream ss;
    ss << alphaStr.toStdString() << processor->alpha;
    ui->alphaLabel->setText(QString::fromStdString(ss.str()));

    ss.str("");
    ss << lambdaStr.toStdString() << processor->lambda_c;
    ui->lambdaLabel->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss << r1Str.toStdString() << processor->r1;
    ui->r1Label->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss << r2Str.toStdString() << processor->r2;
    ui->r2Label->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss << chromStr.toStdString() << processor->chromAttenuation;
    ui->chromLabel->setText(QString::fromStdString(ss.str()));
}

EVMLyprIIRDialog::~EVMLyprIIRDialog()
{
    delete ui;
}

void EVMLyprIIRDialog::on_alphaSlider_valueChanged(int value)
{
    processor->alpha = value;
    std::stringstream ss;
    ss << alphaStr.toStdString() << processor->alpha;
    ui->alphaLabel->setText(QString::fromStdString(ss.str()));
}

void EVMLyprIIRDialog::on_lambdaSlider_valueChanged(int value)
{
    processor->lambda_c = value;
    std::stringstream ss;
    ss << lambdaStr.toStdString() << processor->lambda_c;
    ui->lambdaLabel->setText(QString::fromStdString(ss.str()));
}

void EVMLyprIIRDialog::on_r1Slider_valueChanged(int value)
{
    processor->r1 = value / 100.0;
    std::stringstream ss;
    ss << r1Str.toStdString() << processor->r1;
    ui->r1Label->setText(QString::fromStdString(ss.str()));
}

void EVMLyprIIRDialog::on_r2Slider_valueChanged(int value)
{
    processor->r2 = value / 100.0;
    std::stringstream ss;
    ss << r2Str.toStdString() << processor->r2;
    ui->r2Label->setText(QString::fromStdString(ss.str()));
}

void EVMLyprIIRDialog::on_chromSlider_valueChanged(int value)
{
    processor->chromAttenuation = value / 10.0;
    std::stringstream ss;
    ss << chromStr.toStdString() << processor->chromAttenuation;
    ui->chromLabel->setText(QString::fromStdString(ss.str()));
}
