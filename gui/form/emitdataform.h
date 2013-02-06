#ifndef EMITDATAFORM_H
#define EMITDATAFORM_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"
#include "type/sarja.h"
#include "type/rasti.h"
#include "type/rastidata.h"

#include "model/emitdatamodel.h"

#include "makrot.h"

namespace Ui {
    class EmitDataForm;
}

class EmitDataForm : public QWidget
{
    Q_OBJECT

public:
    explicit EmitDataForm(QWidget *parent);

    ~EmitDataForm();

    void setupForm(const QString& numero, int vuosi, int kuukausi, const QList<RastiData>& rastit, QVariant luettuEmitId = QVariant());
    void setupForm(const QVariant& tulosId);

signals:
    void requestClose(QWidget *widget);
    void tulosLisatty();

private slots:
    void on_closeButton_clicked();

    void on_uusiButton_clicked();

    void on_korvaaButton_clicked();

    void on_sarjaBox_currentIndexChanged(int index);

    void on_suljeTallentamattaButton_clicked();

private:
    Ui::EmitDataForm *ui;

    EmitDataModel *m_emitDataModel;

    QSqlQueryModel *m_tilaModel;
    QSqlQueryModel *m_sarjaModel;
    QSqlQueryModel *m_tulosModel;

    QVariant m_luettuEmitId;
    QVariant m_tulosId;

    QVariant getSarja();
    QVariant getTila();

    void tarkistaKoodi99(const QList<RastiData>& rastit);

    void tarkistaEmit();
    void tarkistaTulos();
    void lataaLuettuEmit();

    void sqlSarja();
    void sqlTila();

    void valitseKilpailija();
    void valitseSarja();
    void asetaAika();

    void naytaTulos();
};

#endif // EMITDATAFORM_H
