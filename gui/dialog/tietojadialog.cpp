#include "tietojadialog.h"
#include "ui_tietojadialog.h"

TietojaDialog::TietojaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TietojaDialog)
{
    ui->setupUi(this);

    ui->buildLabel->setText("Build: " __DATE__ " " __TIME__);
    ui->versionLabel->setText("Versio: " VERSION);
}

TietojaDialog::~TietojaDialog()
{
    delete ui;
}
