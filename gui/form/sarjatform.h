#ifndef SARJATFORM_H
#define SARJATFORM_H

#include <QtWidgets>
#include <QtSql>

#include "delegate/tulospalveludelegate.h"

#include "form/utilform.h"

#include "model/ratamodel.h"

#include "model/rastimodel.h"

#include "type/tapahtuma.h"

#include "makrot.h"

namespace Ui {
    class SarjatForm;
}

class SarjatForm : public UtilForm
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

    void on_yhteislahtoButton_clicked();

private:
    Ui::SarjatForm *ui;

    TulospalveluDelegate *m_delegate;

    QVariant m_sarjaId;

    RataModel *m_sarjaModel;
    RastiModel *m_rastiHeaderModel;
    QHeaderView *m_headerView;
};

#endif // SARJATFORM_H
