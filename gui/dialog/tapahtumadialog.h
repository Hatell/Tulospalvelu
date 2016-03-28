#ifndef TAPAHTUMADIALOG_H
#define TAPAHTUMADIALOG_H

#include <QtWidgets>
#include <QtSql>

#include "type/tapahtuma.h"

#include "makrot.h"

namespace Ui {
    class TapahtumaDialog;
}

class TapahtumaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TapahtumaDialog(QWidget *parent);
    ~TapahtumaDialog();

private slots:
    void on_valitseButton_clicked();

    void on_uusiButton_clicked();

private:
    Ui::TapahtumaDialog *ui;

    QSqlQueryModel *m_tapahtumaModel;

    void sqlTapahtuma();
};

#endif // TAPAHTUMADIALOG_H
