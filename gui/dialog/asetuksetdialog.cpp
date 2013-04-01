#include "asetuksetdialog.h"
#include "ui_asetuksetdialog.h"

AsetuksetDialog::AsetuksetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AsetuksetDialog),
    m_settings()
{
    ui->setupUi(this);

    ui->mainWindowAutoUpdateSelausForm->setChecked(m_settings.value("MainWindow/autoUpdateSelausForm", true).toBool());
    ui->mainWindowAutoUpdateTuloksetForm->setChecked(m_settings.value("MainWindow/autoUpdateTuloksetForm", false).toBool());
    ui->mainWindowCloseOnlyIfCurrentWidget->setChecked(m_settings.value("MainWindow/closeOnlyIfCurrentWidget", true).toBool());

    ui->tulosFormPointSize->setValue(m_settings.value("TulosForm/pointSize", font().pointSize()).toInt());
    ui->tulosFormTulosLabelPointSize->setValue(m_settings.value("TulosForm/tulosLabel.pointSize", 28).toInt());
    ui->tulosFormBeepEnabled->setChecked(m_settings.value("TulosForm/beepEnabled", true).toBool());

    ui->tulosnayttoPointSize->setValue(m_settings.value("TulosnayttoForm/pointSize", 30).toInt());
    ui->tulosnayttoFormTulosLimit->setValue(m_settings.value("TulosnayttoForm/tulosLimit", 3).toInt());
}

AsetuksetDialog::~AsetuksetDialog()
{
    delete ui;
}

void AsetuksetDialog::on_mainWindowAutoUpdateTuloksetForm_toggled(bool checked)
{
    m_settings.setValue("MainWindow/autoUpdateTuloksetForm", checked);
}

void AsetuksetDialog::on_mainWindowAutoUpdateSelausForm_toggled(bool checked)
{
    m_settings.setValue("MainWindow/autoUpdateSelausForm", checked);
}

void AsetuksetDialog::on_mainWindowCloseOnlyIfCurrentWidget_toggled(bool checked)
{
    m_settings.setValue("MainWindow/closeOnlyIfCurrentWidget", checked);
}

void AsetuksetDialog::on_tulosFormPointSize_valueChanged(int arg1)
{
    m_settings.setValue("TulosForm/pointSize", arg1);
}

void AsetuksetDialog::on_tulosFormTulosLabelPointSize_valueChanged(int arg1)
{
    m_settings.setValue("TulosForm/tulosLabel.pointSize", arg1);
}

void AsetuksetDialog::on_tulosFormBeepEnabled_toggled(bool checked)
{
    m_settings.setValue("TulosForm/beepEnabled", checked);
}

void AsetuksetDialog::on_tulosnayttoPointSize_valueChanged(int arg1)
{
    m_settings.setValue("TulosnayttoForm/pointSize", arg1);
}

void AsetuksetDialog::on_tulosnayttoFormTulosLimit_valueChanged(int arg1)
{
    m_settings.setValue("TulosnayttoForm/tulosLimit", arg1);
}
