#include "selausform.h"
#include "ui_selausform.h"

SelausForm::SelausForm(QWidget *parent) :
    UtilForm(parent),
    ui(new Ui::SelausForm)
{
    ui->setupUi(this);
}

SelausForm::~SelausForm()
{
    delete ui;
}

void SelausForm::on_closeButton_clicked()
{
    emit requestClose(this);
}

void SelausForm::updateForm()
{
    qDebug() << "FIXME";
}
