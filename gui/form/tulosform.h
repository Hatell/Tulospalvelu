#ifndef TULOSFORM_H
#define TULOSFORM_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"
#include "type/tulos.h"

#include "makrot.h"

namespace Ui {
    class TulosForm;
}

class TulosForm : public QWidget
{
    Q_OBJECT

public:
    explicit TulosForm(QWidget *parent);
    ~TulosForm();

signals:
    void requestClose(QWidget *widget);
    void requestTulosForm(QVariant tulosId);

private slots:
    void on_closeButton_clicked();

    void on_updateButton_clicked();

    void on_fileButton_clicked();

    void on_tulosAvaaButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::TulosForm *ui;

    QSortFilterProxyModel *m_filterModel;
    QSqlQueryModel *m_tulosModel;

    QList<Sarja*> m_sarjat;
    QMap< QString, QList<Tulos> > m_tulokset;

    QString m_tulosString;
    QString m_valiaikaString;

    void sqlTulokset();

    void sqlTulos();
    void updateTulosEdit();
    void updateValiaikaEdit();
    void updateLehteenEdit();

    QString createValiaika(Sarja* s);
    QString createRastivali(Sarja* s);

    QString timeFormat(const QTime& time) const;
};

#endif // TULOSFORM_H