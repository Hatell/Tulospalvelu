#ifndef SARJATFORM_H
#define SARJATFORM_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"

#include "makrot.h"

namespace Ui {
    class SarjatForm;
}

class SarjatForm : public QWidget
{
    Q_OBJECT

public:
    explicit SarjatForm(QWidget *parent);
    ~SarjatForm();

signals:
    void requestClose(QWidget *widget);

private slots:
    void on_sarjaView_clicked(const QModelIndex &index);

    void on_closeButton_clicked();

    void on_sarjaLisaaButton_clicked();

    void on_sarjaPoistaButton_clicked();

    void on_rastiLisaaButton_clicked();

    void on_rastiPoistaButton_clicked();

private:
    Ui::SarjatForm *ui;

    QVariant m_sarjaId;

    QSqlTableModel *m_sarjaModel;
    QSqlTableModel *m_rastiModel;


    void sqlSarja();
    void sqlRasti();

};

#endif // SARJATFORM_H
