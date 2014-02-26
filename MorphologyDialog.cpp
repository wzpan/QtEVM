#include "MorphologyDialog.h"
#include "ui_MorphologyDialog.h"

MorphologyDialog::MorphologyDialog(QWidget *parent, MorphologyProcessor *processor) :
    QDialog(parent),
    ui(new Ui::MorphologyDialog)
{
    this->processor = processor;
    ui->setupUi(this);

    ui->comboElement->setCurrentIndex(processor->elem);
    ui->sizeSlider->setValue(processor->size);
    ui->sizeLabel->setText(tr("<p>Kernel Size:</p>"
                              "<p><b>%1</b></p>").arg(processor->size));
}

MorphologyDialog::~MorphologyDialog()
{
    delete ui;
}

void MorphologyDialog::on_comboElement_currentIndexChanged(int index)
{
    processor->elem = index;
}

void MorphologyDialog::on_sizeSlider_valueChanged(int value)
{
    processor->size = value;
    ui->sizeLabel->setText(tr("<p>Kernel Size:</p>"
                              "<p><b>%1</b></p>").arg(value));
}

void MorphologyDialog::on_comboType_currentIndexChanged(int index)
{
    processor->type = index;
}
