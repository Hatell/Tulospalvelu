#ifndef SELAUSFORM_H
#define SELAUSFORM_H

#include <QtGui>
#include <QtSql>

#include "form/utilform.h"

#include "makrot.h"

namespace Ui {
    class SelausForm;
}

class SelausForm : public UtilForm
{
    Q_OBJECT

public:
    explicit SelausForm(QWidget *parent = 0);
    ~SelausForm();

public slots:
    void updateForm();

signals:
    void requestClose(QWidget *);

private slots:
    void handleRequestClose();

    void on_ensimmainenButton_clicked();

    void on_edellinenButton_clicked();

    void on_seuraavaButton_clicked();

    void on_viimeinenButton_clicked();

private:
    Ui::SelausForm *ui;

    QSqlQueryModel *m_tulosModel;

    int m_currentRow;

    void sqlTulos();
    void setupTulosForm();
};

#endif // SELAUSFORM_H
