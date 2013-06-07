#include "tulosform.h"
#include "ui_tulosform.h"

TulosForm::TulosForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TulosForm),
    m_settings(),
    m_tulosDataModel(0), // setupForm,
    m_tilaModel(new QSqlQueryModel(this)),
    m_sarjaModel(new QSqlQueryModel(this)),
    m_tulosModel(new QSqlQueryModel(this)),
    m_luettuEmitId(), // setupForm
    m_tulosId(), // setupForm
    m_maaliaika(), // setupForm
    m_allSaved(false),
    m_luettuTulos(true),
    m_canDiscard(false),
    m_canAutoClose(true),
    m_canAutoSave(true)
{
    ui->setupUi(this);
    QFont f = font();
    ui->pointSizeBox->setValue(m_settings.value("TulosForm/pointSize", f.pointSize()).toInt());

    f.setPointSize(m_settings.value("TulosForm/tulosLabel.pointSize", 28).toInt());
    ui->tilaLabel->setFont(f);

    ui->pointSizeBox->setVisible(false);

    ui->tilaBox->setModel(m_tilaModel);
    ui->sarjaBox->setModel(m_sarjaModel);
    ui->tulosView->setModel(m_tulosModel);

    ui->sarjaBox->installEventFilter(this);
    ui->tilaBox->installEventFilter(this);

    setupShortcuts();
}

TulosForm::~TulosForm()
{
    delete ui;
}

void TulosForm::setupShortcuts()
{
    QShortcut *s = 0;

    s = new QShortcut(QKeySequence("Ctrl+1"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutCrtl1()));

    s = new QShortcut(QKeySequence("Ctrl+2"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutCtrl2()));

    s = new QShortcut(QKeySequence("Ctrl+3"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutCtrl3()));

    s = new QShortcut(QKeySequence("Ctrl++"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutCtrlPlus()));

    s = new QShortcut(QKeySequence("Ctrl+-"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutCtrlMinus()));

    s = new QShortcut(QKeySequence("Ctrl+H"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(handleShortcutCtrlH()));
}

void TulosForm::setupForm(const QDateTime& lukuaika, const QString &numero, int vuosi, int kuukausi, const QList<RastiData> &rastit, QVariant luettuEmitId)
{
    m_luettuEmitId = luettuEmitId;
    m_maaliaika = lukuaika;

    tarkistaKoodi99(rastit);

    setAllSaved(false);

    QSqlDatabase::database().transaction();

    m_tulosDataModel = new TulosDataModel(this, numero, vuosi, kuukausi, rastit);

    ui->emitDataView->setModel(m_tulosDataModel);

    ui->emitDataView->expandAll();

    tarkistaEmit();
    tarkistaTulos();
    lataaLuettuEmit();

    valitseKilpailija();

    sqlTila();
    sqlSarja();

    valitseSarja();

    int maali_aikaleima = 0;
    int lukija_aikaleima = 0;

    const Sarja* s = m_tulosDataModel->getSarja();

    foreach (RastiData d, rastit) {
        if (s && s->getMaalirasti().sisaltaa(d.m_rasti)) {
            maali_aikaleima = d.m_aika;
        }

        if (d.m_rasti == 250) {
            lukija_aikaleima = d.m_aika;
        }

        if (d.m_aika == 0 && d.m_rasti != 0) {
            ui->eiNollaustaLabel->setText(_("Suunnistaja ei ole nollannut emittiä!"));
        }
    }

    m_maaliaika = m_maaliaika.addSecs(maali_aikaleima - lukija_aikaleima);

    QSqlDatabase::database().commit();


    // Jos ei ole uusi, tila on Hyväksytty ja Kilpailijalla on nimi, tallennetaan tulos
    if (ui->stackedWidget->currentIndex() == 0 &&
        ui->tilaBox->currentText() == _("Hyväksytty") &&
        !ui->kilpailijaEdit->text().trimmed().isEmpty()) {
        on_saveButton_clicked();
    }
}

void TulosForm::setupForm(const QVariant &tulosId)
{
    m_luettuTulos = false;
    m_canDiscard = true;
    m_canAutoClose = false;
    m_canAutoSave = false;

    m_tulosId = tulosId;

    QSqlDatabase::database().transaction();

    // Haetaan emitDataModel:a varten tiedot
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  l_e.id AS luettu_emit,\n"
                "  t.emit,\n"
                "  e.kuukausi,\n"
                "  e.vuosi,\n"
                "  t.sarja,\n"
                "  t.tila,\n"
                "  t.aika,\n"
                "  k.nimi AS kilpailija,\n"
                "  t.maaliaika\n"
                "FROM tulos AS t\n"
                "  JOIN kilpailija AS k ON t.kilpailija = k.id\n"
                "  JOIN emit AS e ON e.id = t.emit\n"
                "  JOIN luettu_emit AS l_e ON l_e.tulos = t.id\n"
                "WHERE t.tapahtuma = ?\n"
                "  AND t.id = ?\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(m_tulosId);

    SQL_EXEC(query,);

    if (!query.next()) {
        INFO(this, _("Virhe ladatessa tulosta"));
        return;
    }

    QSqlRecord r = query.record();

    m_maaliaika = r.value("maaliaika").toDateTime();

    m_tulosDataModel = new TulosDataModel(
                this,
                r.value("emit").toString(),
                r.value("vuosi").toInt(),
                r.value("kuukausi").toInt(),
                RastiData::luettuRasit(r.value("luettu_emit")),
                0
    );

    ui->emitDataView->setModel(m_tulosDataModel);

    ui->emitDataView->expandAll();

    sqlTila();
    sqlSarja();

    naytaTulos();

    ui->kilpailijaEdit->setText(r.value("kilpailija").toString());

    for (int i = 0; i < m_sarjaModel->rowCount(); i++) {
        if (r.value("sarja") == m_sarjaModel->index(i, 0).data(Qt::EditRole)) {
            ui->sarjaBox->setCurrentIndex(i);
        }
    }

    for (int i = 0; i < m_tilaModel->rowCount(); i++) {
        if (r.value("tila") == m_tilaModel->index(i, 0).data(Qt::EditRole)) {
            ui->tilaBox->setCurrentIndex(i);
        }
    }

    ui->aikaTimeEdit->setTime(r.value("aika").toTime());

    QSqlDatabase::database().commit();

    setAllSaved(true);
}

void TulosForm::sqlTila()
{
    QSqlQuery query;

    query.prepare("SELECT id, nimi FROM tulos_tila");

    SQL_EXEC(query, );

    m_tilaModel->setQuery(query);

    ui->tilaBox->setModelColumn(1);
}

void TulosForm::sqlSarja()
{
    QSqlQuery query;

    query.prepare("SELECT id, nimi FROM sarja WHERE tapahtuma = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query, );

    m_sarjaModel->setQuery(query);

    ui->sarjaBox->setModelColumn(1);
}

void TulosForm::valitseSarja()
{
    QList<Sarja*> sarjat = Sarja::haeSarjat(this);

    // Sarjoja ei ole asetettu
    if (sarjat.count() == 0) {
        return;
    }

    int suurin = 0;
    int suurin_paino = 0;

    QList<RastiData> haettu = m_tulosDataModel->getRastit();

    int sarja_i = 0;

    foreach (Sarja* s, sarjat) {
        int paino = 0;
        int oikeinHaetut = 0;

        int d_i = 1;

        foreach (RastiData d, haettu) {
            if (d.m_rasti == 0) {
                continue;
            }

            foreach (Rasti r, s->getRastit()) {
                if (r.sisaltaa(d.m_rasti)) {
                    paino++;
                }

                if (r.sisaltaa(d.m_rasti) && d_i == r.getNumero()) {
                    paino++;
                    oikeinHaetut++;
                }
            }

            d_i++;
        }

        if (paino > suurin_paino) {
            suurin_paino = paino;
            suurin = sarja_i;
        }

        sarja_i++;
    }

    ui->sarjaBox->setCurrentIndex(suurin);
}

void TulosForm::updateTila()
{
    if (!m_tulosDataModel) {
        return;
    }

    const Sarja *s = m_tulosDataModel->getSarja();

    if (!s) {
        return;
    }

    if (s->isSakkoaika() || m_tulosDataModel->countVirheet() == 0) {
        ui->tilaBox->setCurrentIndex(1);

        return;
    }

    ui->tilaBox->setCurrentIndex(2);

    return;
}

void TulosForm::updateTilaLabel()
{
    if (!m_tulosDataModel) {
        return;
    }

    const Sarja *s = m_tulosDataModel->getSarja();

    if (!s) {
        return;
    }

    QString style;
    QString tila;

    if (ui->tilaBox->currentIndex() == 0) {
        tila = "Avoin";
    } else if (ui->tilaBox->currentIndex() == 1) {
        if (ui->kilpailijaEdit->text().isEmpty()) {
            style = _("QLabel { color: blue }");
        } else {
            style = _("QLabel { color: darkgreen }");
        }

        tila = _("OK");
    } else if (ui->tilaBox->currentIndex() == 2) {
        // Tulos: DNF
        style = _("QLabel { color: red }");
        tila = _("DNF");
    }

    ui->tilaLabel->setStyleSheet(style);
    ui->tilaLabel->setText(_("%1 - %2 - %3").arg(
                               tila,
                               s->getNimi(),
                               m_allSaved ? _("Tallennettu") : _("Muutoksia"))
    );
}

void TulosForm::tarkistaKoodi99(const QList<RastiData> &rastit)
{
    int aika = -1;

    foreach (RastiData d, rastit) {
        if (d.m_rasti == 99) {
            aika = d.m_aika;
            break;
        }
    }

    if (aika > -1) {
        foreach (RastiData d, rastit) {
            if (abs(d.m_aika - aika) <= 5) {
                ui->koodi99Label->setText(_("Emitkoodilla %1 oleva\nrasti on sammumassa").arg(QString::number(d.m_rasti)));
                break;
            }
        }
    }
}

void TulosForm::tarkistaEmit()
{
    QSqlQuery query;

    query.prepare("SELECT * FROM emit WHERE id = ?");

    query.addBindValue(m_tulosDataModel->getNumero());

    SQL_EXEC(query,);

    if (query.next()) {
        return;
    }

    query.prepare("INSERT INTO emit (id, vuosi, kuukausi) VALUES (?, ?, ?)");

    query.addBindValue(m_tulosDataModel->getNumero());
    query.addBindValue(m_tulosDataModel->getVuosi());
    query.addBindValue(m_tulosDataModel->getKuukausi());

    SQL_EXEC(query,);
}

void TulosForm::valitseKilpailija()
{
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  k.nimi\n"
                "FROM kilpailija AS k\n"
                "  JOIN emit ON emit.kilpailija = k.id\n"
                "           AND NOT emit.laina\n"
                "WHERE emit.id = ?"
    );

    query.addBindValue(m_tulosDataModel->getNumero());

    SQL_EXEC(query,);

    if (query.next()) {
        QSqlRecord r = query.record();

        ui->kilpailijaEdit->setText(r.value("nimi").toString());
    }
}

void TulosForm::on_saveButton_clicked()
{
    if (ui->kilpailijaEdit->text().trimmed().isEmpty()) {
        QMessageBox::information(this, _("Tulospalvelu"), _("Syötä kilpailijan nimi."));
        return;
    }

    QSqlDatabase::database().transaction();

    QVariant kilpailijaId;
    QVariant sarjaId = getSarja();
    QVariant tilaId = getTila();

    QSqlQuery query;

    // Tarkistetaan kilpailijan tiedot
    query.prepare("SELECT id FROM kilpailija WHERE nimi = ?");

    query.addBindValue(ui->kilpailijaEdit->text().trimmed());

    SQL_EXEC(query,);

    if (query.next()) {
        kilpailijaId = query.value(0);
    } else {
        query.prepare("INSERT INTO kilpailija (nimi) VALUES (?)");

        query.addBindValue(ui->kilpailijaEdit->text().trimmed());

        SQL_EXEC(query,);

        kilpailijaId = query.lastInsertId();
    }

    // Päivitetään emitin kilpailija
    query.prepare(
                "UPDATE emit SET\n"
                "  kilpailija = ?\n"
                "WHERE id = ?\n"
                "  AND NOT laina\n"
    );

    query.addBindValue(kilpailijaId);
    query.addBindValue(m_tulosDataModel->getNumero());

    SQL_EXEC(query,);

    if (m_tulosId.isNull()) {
        // Luodaan tulos
        query.prepare("INSERT INTO tulos (tapahtuma, emit, kilpailija, sarja, tila, aika, maaliaika) VALUES (?, ?, ?, ?, ?, ?, ?)");

        query.addBindValue(Tapahtuma::tapahtuma()->id());
        query.addBindValue(m_tulosDataModel->getNumero());
        query.addBindValue(kilpailijaId);
        query.addBindValue(sarjaId);
        query.addBindValue(tilaId);
        query.addBindValue(ui->aikaTimeEdit->time());
        query.addBindValue(m_maaliaika);

        SQL_EXEC(query,);

        m_tulosId = query.lastInsertId();
    } else {
        // Tulos oli jo tallennettu => Poistetaan väliajat
        query.prepare("DELETE FROM valiaika WHERE tulos = ?");

        query.addBindValue(m_tulosId);

        SQL_EXEC(query,);

        // Päivitetään tiedot
        query.prepare("UPDATE tulos SET tapahtuma = ?, emit = ?, kilpailija = ?, sarja = ?, tila = ?, aika = ? WHERE id = ?");

        query.addBindValue(Tapahtuma::tapahtuma()->id());
        query.addBindValue(m_tulosDataModel->getNumero());
        query.addBindValue(kilpailijaId);
        query.addBindValue(sarjaId);
        query.addBindValue(tilaId);
        query.addBindValue(ui->aikaTimeEdit->time());
        query.addBindValue(m_tulosId);

        SQL_EXEC(query,);
    }

    query.prepare("INSERT INTO valiaika (tulos, numero, koodi, aika) VALUES (?, ?, ?, ?)");

    foreach (Data d, m_tulosDataModel->getValiajat()) {
        query.addBindValue(m_tulosId);
        query.addBindValue(d.a);
        query.addBindValue(d.b);
        query.addBindValue(QTime(0,0).addSecs(d.c.toInt()));

        SQL_EXEC(query,);
    }

    query.prepare("UPDATE luettu_emit SET tulos = ? WHERE id = ?");

    query.addBindValue(m_tulosId);
    query.addBindValue(m_luettuEmitId);

    SQL_EXEC(query,);

    QSqlDatabase::database().commit();

    setAllSaved(true);

    emit tulosTallennettu();
}

QVariant TulosForm::getSarja()
{
    return m_sarjaModel->index(ui->sarjaBox->currentIndex(), 0).data(Qt::EditRole);
}

QVariant TulosForm::getTila()
{
    return m_tilaModel->index(ui->tilaBox->currentIndex(), 0).data(Qt::EditRole);
}

void TulosForm::lataaLuettuEmit()
{
    if (!m_luettuEmitId.isNull()) {
        return;
    }

    QSqlQuery query;

    query.prepare("INSERT INTO luettu_emit (tapahtuma, emit, luettu) VALUES (?, ?, ?)");

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(m_tulosDataModel->getNumero());
    query.addBindValue(QDateTime::currentDateTime());

    SQL_EXEC(query,);

    m_luettuEmitId = query.lastInsertId();

    query.prepare("INSERT INTO luettu_emit_rasti (luettu_emit, numero, koodi, aika) VALUES (?, ?, ?, ?)");

    int nro = 1;
    foreach (RastiData d, m_tulosDataModel->getRastit()) {
        query.addBindValue(m_luettuEmitId);
        query.addBindValue(nro);
        query.addBindValue(d.m_rasti);
        query.addBindValue(d.m_aika);

        SQL_EXEC(query,);

        nro++;
    }
}

void TulosForm::tarkistaTulos()
{
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  t.id,\n"
                "  t.emit,\n"
                "  tila.nimi AS tila,\n"
                "  t.aika,\n"
                "  k.nimi AS kilpailija\n"
                "FROM tulos AS t\n"
                "  JOIN tulos_tila AS tila ON tila.id = t.tila\n"
                "  JOIN kilpailija AS k ON k.id = t.kilpailija\n"
                "WHERE t.tapahtuma = ?\n"
                "  AND t.emit = ?\n"
                "  AND NOT t.poistettu\n"
                "ORDER BY t.id DESC"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(m_tulosDataModel->getNumero());

    SQL_EXEC(query,);

    m_tulosModel->setQuery(query);

    if (m_tulosModel->rowCount() == 0) {
        naytaTulos();

        emit tulosLisatty();

        return;
    }

    // Valitaan ensimmäinen tulos oletuksena.
    ui->tulosView->selectionModel()->select(ui->tulosView->model()->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

    ui->stackedWidget->setCurrentIndex(1);
}

void TulosForm::on_uusiButton_clicked()
{
    ui->kilpailijaEdit->setText("");

    naytaTulos();

    emit tulosLisatty();

    handleShortcutCtrl2();
}

void TulosForm::on_korvaaButton_clicked()
{
    foreach (QModelIndex index, ui->tulosView->selectionModel()->selectedRows(0)) {
        m_tulosId = index.data(Qt::EditRole);

        break;
    }

    if (m_tulosId.isNull()) {
        INFO(this, _("Valitse tulos, joka korvataan."));
        return;
    }

    naytaTulos();
}

void TulosForm::on_sarjaBox_currentIndexChanged(int index)
{
    m_tulosDataModel->setSarja(Sarja::haeSarja(m_tulosDataModel, m_sarjaModel->index(index, 0).data(Qt::EditRole)));
    ui->emitDataView->expandAll();

    ui->aikaTimeEdit->setTime(m_tulosDataModel->getAika());

    updateTila();
    updateTilaLabel();

    setAllSaved(false);
}

void TulosForm::on_kilpailijaEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    updateTilaLabel();

    setAllSaved(false);
}

void TulosForm::on_tilaBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    updateTilaLabel();

    setAllSaved(false);
}

void TulosForm::on_closeButton_clicked()
{
    emit requestClose(this);
}

void TulosForm::naytaTulos()
{
    ui->stackedWidget->setCurrentIndex(0);

    QShortcut *s = 0;

    s = new QShortcut(QKeySequence("Alt+ENTER"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(on_saveButton_clicked()));

    s = new QShortcut(QKeySequence("Alt+RETURN"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(on_saveButton_clicked()));
}

void TulosForm::handleShortcutCrtl1()
{
    ui->tilaBox->setFocus();
    ui->tilaBox->showPopup();
}

void TulosForm::handleShortcutCtrl2()
{
    ui->kilpailijaEdit->setFocus();
    ui->kilpailijaEdit->selectAll();
}

void TulosForm::handleShortcutCtrl3()
{
    ui->sarjaBox->setFocus();
    ui->sarjaBox->showPopup();
}

void TulosForm::handleShortcutCtrlH()
{
    ui->tilaBox->setCurrentIndex(1);
}

void TulosForm::handleShortcutCtrlPlus()
{
    ui->pointSizeBox->stepUp();
}

void TulosForm::handleShortcutCtrlMinus()
{
    ui->pointSizeBox->stepDown();
}

void TulosForm::setAllSaved(bool b)
{
    m_allSaved = b;

    ui->closeButton->setEnabled(m_canDiscard || b);
    ui->saveButton->setEnabled(!b);

    updateTilaLabel();
}

bool TulosForm::isAllSaved() const
{
    return m_allSaved;
}

bool TulosForm::isLuettuTulos() const
{
    return m_luettuTulos;
}

bool TulosForm::canAutoClose() const
{
    return m_canAutoClose && m_allSaved;
}

void TulosForm::on_pointSizeBox_valueChanged(int arg1)
{
    QFont f = font();
    m_settings.setValue("TulosForm/pointSize", arg1);
    f.setPointSize(arg1);
    setFont(f);
}

void TulosForm::checkFocus()
{
    if (ui->kilpailijaEdit->text().trimmed().isEmpty()) {
        ui->kilpailijaEdit->setFocus();
    }
}

void TulosForm::on_kilpailijaEdit_returnPressed()
{
    if (ui->saveButton->isEnabled()) {
        ui->saveButton->click();
    }
}

void TulosForm::on_aikaTimeEdit_timeChanged(const QTime &date)
{
    Q_UNUSED(date);
    setAllSaved(false);
}

void TulosForm::saveForm()
{
    on_saveButton_clicked();
}

bool TulosForm::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->sarjaBox || obj == ui->tilaBox) {
        if (e->type() == QEvent::KeyRelease) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);

            if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
                if (ui->saveButton->isEnabled()) {
                    ui->saveButton->click();
                }
            }
        }
    }

    return QWidget::eventFilter(obj, e);
}
