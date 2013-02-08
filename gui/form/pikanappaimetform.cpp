#include "pikanappaimetform.h"
#include "ui_pikanappaimetform.h"

PikanappaimetForm::PikanappaimetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PikanappaimetForm)
{
    ui->setupUi(this);
}

PikanappaimetForm::~PikanappaimetForm()
{
    delete ui;
}
