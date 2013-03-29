#ifndef TIETOJADIALOG_H
#define TIETOJADIALOG_H

#include <QDialog>

namespace Ui {
    class TietojaDialog;
}

class TietojaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TietojaDialog(QWidget *parent);
    ~TietojaDialog();

private:
    Ui::TietojaDialog *ui;
};

#endif // TIETOJADIALOG_H
