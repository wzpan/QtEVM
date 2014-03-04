#include "ParamDialog.h"
#include <string.h>
#include "ui_ParamDialog.h"

ParamDialog::ParamDialog(QWidget *parent, VideoProcessor *processor) :
    QDialog(parent),
    ui(new Ui::ParamDialog)
{
    ui->setupUi(this);

    this->processor = processor;

    alphaStr = ui->alphaLabel->text();
    lambdaStr = ui->lambdaLabel->text();
    flStr = ui->flLabel->text();
    fhStr = ui->fhLabel->text();
    chromStr = ui->chromLabel->text();

    std::stringstream ss;
    ss << alphaStr.toStdString() << processor->alpha;
    ui->alphaLabel->setText(QString::fromStdString(ss.str()));

    ss.str("");
    ss << lambdaStr.toStdString() << processor->lambda_c;
    ui->lambdaLabel->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss << flStr.toStdString() << processor->fl;
    ui->flLabel->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss << fhStr.toStdString() << processor->fh;
    ui->fhLabel->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss << chromStr.toStdString() << processor->chromAttenuation;
    ui->chromLabel->setText(QString::fromStdString(ss.str()));
}

ParamDialog::~ParamDialog()
{
    delete ui;
}

void ParamDialog::on_alphaSlider_valueChanged(int value)
{
    processor->alpha = value;
    std::stringstream ss;
    ss << alphaStr.toStdString() << processor->alpha;
    ui->alphaLabel->setText(QString::fromStdString(ss.str()));
}

void ParamDialog::on_lambdaSlider_valueChanged(int value)
{
    processor->lambda_c = value;
    std::stringstream ss;
    ss << lambdaStr.toStdString() << processor->lambda_c;
    ui->lambdaLabel->setText(QString::fromStdString(ss.str()));
}

void ParamDialog::on_flSlider_valueChanged(int value)
{
    processor->fl = value / 100.0;
    std::stringstream ss;
    ss << flStr.toStdString() << processor->fl;
    ui->flLabel->setText(QString::fromStdString(ss.str()));
}

void ParamDialog::on_fhSlider_valueChanged(int value)
{
    processor->fh = value / 100.0;
    std::stringstream ss;
    ss << fhStr.toStdString() << processor->fh;
    ui->fhLabel->setText(QString::fromStdString(ss.str()));
}

void ParamDialog::on_chromSlider_valueChanged(int value)
{
    processor->chromAttenuation = value / 10.0;
    std::stringstream ss;
    ss << chromStr.toStdString() << processor->chromAttenuation;
    ui->chromLabel->setText(QString::fromStdString(ss.str()));
}
