#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_databaseOK(false),
    m_tuloksia(0),
    m_kilpailijoita(0),
    m_tuloksiaLabel(new QLabel(this)),
    m_kilpailijoitaLabel(new QLabel(this)),
    m_serialStatus(new QLabel(this)),
    m_testEmitReader(0),
    m_serialEmitReader(0),
    m_pikanappaimetForm(0)
{
    ui->setupUi(this);

    connectDatabase();
    buildDatabase();

    setupTapahtuma();

    setupShortcuts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (m_testEmitReader) {
        m_testEmitReader->close();
    }

    if (m_serialEmitReader) {
        m_serialEmitReader->closeSerial();
        m_serialEmitReader->close();
    }

    if (m_pikanappaimetForm) {
        m_pikanappaimetForm->close();
    }

    // Varmistetaan, että kaikki transaktiot tulee tietokantaan.
    QSqlDatabase::database().commit();
}

void MainWindow::setupShortcuts()
{
    QShortcut *s = 0;

    s = new QShortcut(QKeySequence("Alt+UP"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutAltUP()));

    s = new QShortcut(QKeySequence("Alt+DOWN"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutAltDOWN()));

    s = new QShortcut(QKeySequence("Alt+LEFT"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutAltLEFT()));

    s = new QShortcut(QKeySequence("Alt+RIGHT"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutAltRIGHT()));
}

void MainWindow::setupTapahtuma()
{
    if (ui->tabWidget->count() > 0) {
        INFO(this, _("Sulje kaikki tabit ennen tapahtuman vaihtamista."));
        return;
    }

    TapahtumaDialog d(this);

    if (d.exec() != QDialog::Accepted) {
        return;
    }

    setupDatabase();

    this->setWindowTitle(_("Tulospalvelu - %1").arg(Tapahtuma::tapahtuma()->nimi()));
    m_serialStatus->setText("Serial: - ");
    statusBar()->addPermanentWidget(m_serialStatus);
    statusBar()->addPermanentWidget(m_kilpailijoitaLabel);
    statusBar()->addPermanentWidget(m_tuloksiaLabel);

    updateStatus();
}

void MainWindow::connectDatabase()
{
#ifdef USE_MYSQL
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setUserName("tulospalvelu");
    db.setDatabaseName("tulospalvelu");

#else
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("tulospalvelu.sqlite3");
#endif

    if (!db.open()) {
        INFO(this, _("Tietokannan avaus ei onnistunut"));
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM tulos WHERE tapahtuma = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query, );

    m_databaseOK = true;
}

void MainWindow::setupDatabase()
{
    QSqlDatabase::database().transaction();

    QSqlQuery query;

    query.prepare("SELECT COUNT(*) FROM tulos WHERE tapahtuma = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query, );

    query.next();

    m_tuloksia = query.value(0).toInt();

    query.prepare("SELECT l.*, e.vuosi, e.kuukausi FROM luettu_emit AS l JOIN emit AS e ON e.id = l.emit WHERE l.tapahtuma = ? AND l.tulos IS NULL");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    while (query.next()) {
        QSqlRecord r = query.record();

        ui->tabWidget->addTab(
            newTulosForm(
                r.value("emit").toString(),
                r.value("vuosi").toInt(),
                r.value("kuukausi").toInt(),
                RastiData::luettuRasit(r.value("id")),
                r.value("id")
            ),
            r.value("emit").toString()
        );
    }

    updateKilpailijoita();

    QSqlDatabase::database().commit();

    updateStatus();
}

void MainWindow::setupTestEmitReader()
{
    if (!m_testEmitReader) {
        m_testEmitReader = new TestEmitReaderWidget();

        connect(m_testEmitReader, SIGNAL(readEmit(QString,int,int,QList<RastiData>)),
                this, SLOT(handleReadEmit(QString,int,int,QList<RastiData>)));
    }
}

void MainWindow::setupSerialEmitReader()
{
    if (!m_serialEmitReader) {
        m_serialEmitReader = new SerialEmitReaderWidget();

        connect(m_serialEmitReader, SIGNAL(readEmit(QString,int,int,QList<RastiData>)),
                this, SLOT(handleReadEmit(QString,int,int,QList<RastiData>)));
        connect(m_serialEmitReader, SIGNAL(statusChanged(QString)),
                m_serialStatus, SLOT(setText(QString)));
    }
}

void MainWindow::handleReadEmit(QString numero, int vuosi, int kuukausi, QList<RastiData> rastit)
{
    ui->tabWidget->addTab(newTulosForm(numero, vuosi, kuukausi, rastit), numero);

    TulosForm *f = qobject_cast<TulosForm*>(ui->tabWidget->currentWidget());

    // Mikäli nykyinen tab on TulosForm ja sen voi sulkea, suljetaan se
    if (f && f->canAutoClose()) {
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(f));
    }

    updateStatus();
}

void MainWindow::handleRequestOpenTulosForm(QVariant tulosId)
{
    ui->tabWidget->addTab(newTulosForm(tulosId), _("Tulos: %1").arg(tulosId.toString()));

    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

TulosForm * MainWindow::newTulosForm(const QString &numero, int vuosi, int kuukausi, const QList<RastiData> &rastit, QVariant luettuEmitId)
{
    TulosForm *f = new TulosForm(this);

    connect(f, SIGNAL(requestClose(QWidget*)),
            this, SLOT(handleRequestClose(QWidget*)));

    connect(f, SIGNAL(requestOpenTulokset()),
            this, SLOT(handleRequestOpenTulokset()));

    connect(f, SIGNAL(tulosLisatty()),
            this, SLOT(handleTulosLisatty()));

    f->setupForm(numero, vuosi, kuukausi, rastit, luettuEmitId);

    return f;
}

TulosForm * MainWindow::newTulosForm(QVariant tulosId)
{
    TulosForm *f = new TulosForm(this);

    connect(f, SIGNAL(requestClose(QWidget*)),
            this, SLOT(handleRequestClose(QWidget*)));

    connect(f, SIGNAL(requestOpenTulokset()),
            this, SLOT(handleRequestOpenTulokset()));

    //connect(f, SIGNAL(tulosLisatty()),
    //        this, SLOT(handleTulosLisatty()));

    f->setupForm(tulosId);

    return f;
}

SarjatForm * MainWindow::newSarjatForm()
{
    SarjatForm *f = new SarjatForm(this);

    connect(f, SIGNAL(requestClose(QWidget *)),
            this, SLOT(handleRequestClose(QWidget*)));

    return f;
}

TuloksetForm * MainWindow::newTuloksetForm()
{
    TuloksetForm *f = new TuloksetForm(this);

    connect(f, SIGNAL(requestClose(QWidget *)),
            this, SLOT(handleRequestClose(QWidget*)));

    connect(f, SIGNAL(requestTulosForm(QVariant)),
            this, SLOT(handleRequestOpenTulosForm(QVariant)));

    return f;
}

SelausForm * MainWindow::newSelausForm()
{
    SelausForm *f = new SelausForm(this);

    connect(f, SIGNAL(requestClose(QWidget *)),
            this, SLOT(handleRequestClose(QWidget*)));

    return f;
}

void MainWindow::handleRequestClose(QWidget *widget)
{
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(widget));
    statusBar()->showMessage("Tallennettu", 500);
}

void MainWindow::handleRequestOpenTulokset()
{
    on_actionTulokset_triggered();
}

void MainWindow::updateStatus()
{
    m_tuloksiaLabel->setText("Tuloksia luettu " + QString::number(m_tuloksia));
    m_kilpailijoitaLabel->setText("Kilpailijoita: " + QString::number(m_kilpailijoita));
}

void MainWindow::updateKilpailijoita()
{
    QSqlQuery query;

    query.prepare("SELECT COUNT(*) FROM kilpailija");

    SQL_EXEC(query, );

    query.next();

    m_kilpailijoita = query.value(0).toInt();
}

void MainWindow::on_actionTestEmitReader_triggered()
{
    setupTestEmitReader();

    m_testEmitReader->show();
}


void MainWindow::on_actionSerialEmitReader_triggered()
{
    setupSerialEmitReader();

    m_serialEmitReader->show();
}


void MainWindow::on_actionSarjat_triggered()
{
    SarjatForm *f = 0;

    int ind = 0;

    for (ind = 0; ind < ui->tabWidget->count(); ind++) {
        QWidget *wid = ui->tabWidget->widget(ind);

        if (qobject_cast<SarjatForm*>(wid)) {
            f = qobject_cast<SarjatForm*>(wid);
        }

        if (!qobject_cast<UtilForm*>(wid)) {
            break;
        }
    }

    if (!f) {
        f = newSarjatForm();
        ui->tabWidget->insertTab(ind, f, _("Sarjat"));
    } else {
        ind--;
    }

    ui->tabWidget->setCurrentIndex(ind);
}


void MainWindow::on_actionTulokset_triggered()
{
    TuloksetForm *f = qobject_cast<TuloksetForm*>(ui->tabWidget->widget(0));

    if (!f) {
        f = newTuloksetForm();
        ui->tabWidget->insertTab(0, f, _("Tulokset"));
    } else {
        f->updateForm();
    }

    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_actionTulosten_selaus_triggered()
{
    int i = 0;

    if (qobject_cast<TuloksetForm*>(ui->tabWidget->widget(0))) {
        i++;
    }

    SelausForm *f = qobject_cast<SelausForm*>(ui->tabWidget->widget(i));

    if (!f) {
        f = newSelausForm();
        ui->tabWidget->insertTab(i, f, _("Selaus"));
    } else {
        f->updateForm();
    }

    ui->tabWidget->setCurrentIndex(i);
}

void MainWindow::on_actionTietoja_triggered()
{
    TietojaDialog d(this);

    d.exec();
}

void MainWindow::buildDatabase()
{
    if (m_databaseOK) {
        return;
    }

    if (!QSqlDatabase::database().isOpen()) {
        INFO(this, _("Tietokannan alustus ei onnistu, koska ei ole yhteyttä tietokantaan!"));
        return;
    }

    QSqlDatabase::database().transaction();

#ifdef USE_MYSQL
    Tietokanta::buildMySQL();
#else
    Tietokanta::buildSQLite();
#endif

    QSqlDatabase::database().commit();

    m_databaseOK = true;
}


void MainWindow::on_actionVie_kilpailijatietokanta_triggered()
{
    QString fn = QFileDialog::getSaveFileName(this, _("Tulospalvelu - Tallenna kilpailijatietokanta."), _("kilpdat.db"), _("*.db"));

    if (fn.isNull()) {
        return;
    }

    QFile file(fn);

    if (file.exists()) {
        file.remove();
    }

    Tietokanta::luoKilpailijatietokanta(fn);
}

void MainWindow::on_actionTuo_kilpailijatietokanta_triggered()
{
    QString fn = QFileDialog::getOpenFileName(this, _("Tulospalvelu - Lataa kilpailijatietokanta."), _("kilpdat.db"), _("*.db"));

    if (fn.isNull()) {
        return;
    }

    if (Tietokanta::tuoKilpailijatietokanta(fn)) {
        updateKilpailijoita();
        updateStatus();

        INFO(this, _("Kilpailijatietokannan tuonti onnistui."));
    } else {
        INFO(this, _("Kilpailijatietokannan tuonti epäonnistui."));
    }
}


void MainWindow::on_actionTapahtuma_hallinta_triggered()
{
    setupTapahtuma();
}

void MainWindow::handleShortcutAltUP()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

void MainWindow::handleShortcutAltDOWN()
{
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::handleShortcutAltRIGHT()
{
    int newIndex = ui->tabWidget->currentIndex() + 1;

    if (newIndex < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(newIndex);
    }
}

void MainWindow::handleShortcutAltLEFT()
{
    int newIndex = ui->tabWidget->currentIndex() - 1;

    if (newIndex > -1) {
        ui->tabWidget->setCurrentIndex(newIndex);
    }
}

void MainWindow::handleTulosLisatty()
{
    m_tuloksia++;

    updateStatus();
}


void MainWindow::on_actionVie_tulokset_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, _("Tulospalvelu - Tallenna tulokset."), _("tulosdat.db"), _("*.db"));

    if (fileName.isNull()) {
        return;
    }

    QFile file(fileName);

    if (file.exists()) {
        file.remove();
    }

    Tietokanta::vieTulokset(Tapahtuma::tapahtuma(), fileName);
}

void MainWindow::on_actionTuo_tulokset_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, _("Tulospalvelu - Tuo tulokset."), _("tulosdat.db"), _("*.db"));

    if (fileName.isNull()) {
        return;
    }

    if (Tietokanta::tuoTulokset(Tapahtuma::tapahtuma(), fileName)) {
        updateKilpailijoita();
        updateStatus();

        INFO(this, _("Tulosten tuonti onnistui."));
    } else {
        INFO(this, _("Tulosten tuonti epäonnistui."));
    }
}

void MainWindow::on_actionPikan_pp_imet_triggered()
{
    if (!m_pikanappaimetForm) {
        m_pikanappaimetForm = new PikanappaimetForm();
    }

    m_pikanappaimetForm->show();
}

void MainWindow::on_tabWidget_currentChanged(QWidget *arg1)
{
    TulosForm *f = qobject_cast<TulosForm*>(arg1);

    if (f) {
        f->checkFocus();
    }
}
