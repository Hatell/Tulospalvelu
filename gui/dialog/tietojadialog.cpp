#include "tietojadialog.h"
#include "ui_tietojadialog.h"

TietojaDialog::TietojaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TietojaDialog)
{
    ui->setupUi(this);
}

TietojaDialog::~TietojaDialog()
{
    delete ui;
}
