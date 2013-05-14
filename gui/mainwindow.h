#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtSql>

#include "type/tietokanta.h"
#include "type/rastidata.h"

#include "dialog/tietojadialog.h"
#include "dialog/tapahtumadialog.h"
#include "dialog/asetuksetdialog.h"
#include "dialog/sarjavalintadialog.h"

#include "form/utilform.h"
#include "form/tulosform.h"
#include "form/sarjatform.h"
#include "form/tuloksetform.h"
#include "form/pikanappaimetform.h"
#include "form/selausform.h"
#include "form/vuokraemititform.h"
#include "form/tulosnayttoform.h"

#include "widget/testemitreaderwidget.h"
#include "widget/serialemitreaderwidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSettings m_settings;

    bool m_databaseOK;
    int m_tuloksia;
    int m_kilpailijoita;

    QLabel *m_tuloksiaLabel;
    QLabel *m_serialStatus;
    TestEmitReaderWidget *m_testEmitReader;
    SerialEmitReaderWidget *m_serialEmitReader;
    PikanappaimetForm *m_pikanappaimetForm;
    VuokraEmititForm *m_vuokraEmititForm;
    TulosnayttoForm *m_tulosnayttoForm;

    TulosForm* newTulosForm(const QDateTime& lukuaika, const QString& numero, int vuosi, int kuukausi, const QList<RastiData>& rastit, QVariant luettuEmitId = QVariant());
    TulosForm* newTulosForm(QVariant tulosId);
    SarjatForm* newSarjatForm();
    TuloksetForm* newTuloksetForm();
    SelausForm* newSelausForm();

    void connectDatabase();
    void setupDatabase();
    void buildDatabase();

    void setupShortcuts();
    void setupTapahtuma();

    void setupTestEmitReader();
    void setupTulosnaytto();

    void updateStatus();
    void updateKilpailijoita();

    void closeEvent(QCloseEvent *);

private slots:
    void handleReadEmit(QDateTime lukuaika, QString numero, int vuosi, int kuukausi, QList<RastiData> rastit);
    void handleRequestClose(QWidget *widget);
    void handleRequestOpenTulosForm(QVariant tulosId);
    void handleTulosTallennettu();
    void handleTulosLisatty();

    void handleShortcutAltUP();
    void handleShortcutAltDOWN();
    void handleShortcutAltRIGHT();
    void handleShortcutAltLEFT();
    void handleShortcutEsc();

    void setupSerialMenu();
    void handleSerialMenu(QAction *a);

    void on_actionTestEmitReader_triggered();
    void on_actionSarjat_triggered();
    void on_actionTulokset_triggered();
    void on_actionTietoja_triggered();
    void on_actionVie_kilpailijatietokanta_triggered();
    void on_actionTuo_kilpailijatietokanta_triggered();
    void on_actionTapahtuma_hallinta_triggered();
    void on_actionVie_tulokset_triggered();
    void on_actionTuo_tulokset_triggered();
    void on_actionPikan_pp_imet_triggered();
    void on_actionTulosten_selaus_triggered();
    void on_tabWidget_currentChanged(QWidget *arg1);
    void on_actionVuokraEmitit_triggered();
    void on_actionTulosnaytto_triggered();
    void on_actionAsetukset_triggered();
    void on_actionPaivitaValiajat_triggered();
};

#endif // MAINWINDOW_H
