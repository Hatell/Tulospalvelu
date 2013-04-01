#ifndef TULOSNAYTTOFORM_H
#define TULOSNAYTTOFORM_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"

#include "makrot.h"

namespace Ui {
    class TulosnayttoForm;
}

class TulosnayttoForm : public QWidget
{
    Q_OBJECT

public:
    explicit TulosnayttoForm(QWidget *parent = 0);
    ~TulosnayttoForm();

    void updateForm();

public slots:
    void show();

private:
    Ui::TulosnayttoForm *ui;

    QSettings m_settings;

    void setupFont();

    void sqlTulos();
};

#endif // TULOSNAYTTOFORM_H
