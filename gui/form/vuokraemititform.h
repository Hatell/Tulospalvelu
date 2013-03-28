#ifndef VUOKRAEMITITFORM_H
#define VUOKRAEMITITFORM_H

#include <QtGui>
#include <QtSql>

#include "makrot.h"

namespace Ui {
    class VuokraEmititForm;
}

class VuokraEmititForm : public QWidget
{
    Q_OBJECT

public:
    explicit VuokraEmititForm(QWidget *parent = 0);
    ~VuokraEmititForm();

private slots:
    void on_naytaKaikkiBox_clicked();

    void on_lisaaButton_clicked();

    void on_poistaButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::VuokraEmititForm *ui;

    QSqlTableModel *m_emitModel;

    void sqlEmit();
    void lisaaLainaEmit();
};

#endif // VUOKRAEMITITFORM_H
