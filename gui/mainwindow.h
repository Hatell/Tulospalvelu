#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtSql>

#include "type/tietokanta.h"
#include "type/rastidata.h"

#include "dialog/tietojadialog.h"
#include "dialog/tapahtumadialog.h"

#include "form/tulosform.h"
#include "form/sarjatform.h"
#include "form/tuloksetform.h"

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

    bool m_databaseOK;
    int m_tuloksia;
    int m_kilpailijoita;

    QLabel *m_tuloksiaLabel;
    QLabel *m_kilpailijoitaLabel;
    QLabel *m_serialStatus;
    TestEmitReaderWidget *m_testEmitReader;
    SerialEmitReaderWidget *m_serialEmitReader;

    TulosForm* newTulosForm(const QString& numero, int vuosi, int kuukausi, const QList<RastiData>& rastit, QVariant luettuEmitId = QVariant());
    TulosForm* newTulosForm(QVariant tulosId);
    SarjatForm* newSarjatForm();
    TuloksetForm* newTuloksetForm();

    void connectDatabase();
    void setupDatabase();
    void buildDatabase();

    void setupShortcuts();
    void setupTapahtuma();

    void setupTestEmitReader();
    void setupSerialEmitReader();

    void updateStatus();
    void updateKilpailijoita();

    void closeEvent(QCloseEvent *);

private slots:
    void handleReadEmit(QString numero, int vuosi, int kuukausi, QList<RastiData> rastit);
    void handleRequestClose(QWidget *widget);
    void handleRequestOpenTulokset();
    void handleRequestOpenTulosForm(QVariant tulosId);
    void handleTulosLisatty();

    void handleShortcutAltUP();
    void handleShortcutAltDOWN();
    void handleShortcutAltRIGHT();
    void handleShortcutAltLEFT();

    void on_actionTestEmitReader_triggered();
    void on_actionSerialEmitReader_triggered();
    void on_actionSarjat_triggered();
    void on_actionTulokset_triggered();
    void on_actionTietoja_triggered();
    void on_actionVie_kilpailijatietokanta_triggered();
    void on_actionTuo_kilpailijatietokanta_triggered();
    void on_actionTapahtuma_hallinta_triggered();
    void on_actionVie_tulokset_triggered();
    void on_actionTuo_tulokset_triggered();
};

#endif // MAINWINDOW_H
