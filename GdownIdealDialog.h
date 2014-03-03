#ifndef GDOWNIDEALDIALOG_H
#define GDOWNIDEALDIALOG_H

#include <QDialog>

namespace Ui {
class GdownIdealDialog;
}

class GdownIdealDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GdownIdealDialog(QWidget *parent = 0);
    ~GdownIdealDialog();

private:
    Ui::GdownIdealDialog *ui;
};

#endif // GDOWNIDEALDIALOG_H
