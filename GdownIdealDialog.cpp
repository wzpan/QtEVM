#include "GdownIdealDialog.h"
#include "ui_GdownIdealDialog.h"

GdownIdealDialog::GdownIdealDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GdownIdealDialog)
{
    ui->setupUi(this);
}

GdownIdealDialog::~GdownIdealDialog()
{
    delete ui;
}
