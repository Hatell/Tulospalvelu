#ifndef TULOSFORM_H
#define TULOSFORM_H

#include <QtGui>
#include <QtSql>

#include "type/tapahtuma.h"
#include "type/sarja.h"
#include "type/rasti.h"
#include "type/rastidata.h"

#include "model/emitdatamodel.h"

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

    void setupShortcuts();
    void setupForm(const QString& numero, int vuosi, int kuukausi, const QList<RastiData>& rastit, QVariant luettuEmitId = QVariant());
    void setupForm(const QVariant& tulosId);

signals:
    void requestOpenTulokset();
    void requestClose(QWidget *widget);
    void tulosLisatty();

private slots:
    void on_closeButton_clicked();

    void on_uusiButton_clicked();

    void on_korvaaButton_clicked();

    void on_sarjaBox_currentIndexChanged(int index);

    void on_suljeTallentamattaButton_clicked();

    void on_tuloksetButton_clicked();

    void handleShortcutCrtl1();
    void handleShortcutCtrl2();
    void handleShortcutCtrl3();

private:
    Ui::TulosForm *ui;

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

#endif // TULOSFORM_H
