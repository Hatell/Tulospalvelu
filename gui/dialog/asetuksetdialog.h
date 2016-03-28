#ifndef ASETUKSETDIALOG_H
#define ASETUKSETDIALOG_H

#include <QtWidgets>

#include "makrot.h"

namespace Ui {
    class AsetuksetDialog;
}

class AsetuksetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AsetuksetDialog(QWidget *parent);
    ~AsetuksetDialog();

private slots:
    void on_mainWindowAutoUpdateTuloksetForm_toggled(bool checked);

    void on_mainWindowAutoUpdateSelausForm_toggled(bool checked);

    void on_mainWindowCloseOnlyIfCurrentWidget_toggled(bool checked);

    void on_tulosFormPointSize_valueChanged(int arg1);

    void on_tulosFormTulosLabelPointSize_valueChanged(int arg1);

    void on_tulosFormBeepEnabled_toggled(bool checked);

    void on_tulosnayttoPointSize_valueChanged(int arg1);

    void on_tulosnayttoFormTulosLimit_valueChanged(int arg1);

private:
    Ui::AsetuksetDialog *ui;

    QSettings m_settings;
};

#endif // ASETUKSETDIALOG_H
