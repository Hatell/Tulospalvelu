#include "emitdataform.h"
#include "ui_emitdataform.h"

EmitDataForm::EmitDataForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmitDataForm),
    m_emitDataModel(0), // setupForm,
    m_tilaModel(new QSqlQueryModel(this)),
    m_sarjaModel(new QSqlQueryModel(this)),
    m_tulosModel(new QSqlQueryModel(this)),
    m_luettuEmitId(), // setupForm
    m_tulosId() // setupForm
{
    ui->setupUi(this);

    ui->tilaBox->setModel(m_tilaModel);
    ui->sarjaBox->setModel(m_sarjaModel);
    ui->tulosView->setModel(m_tulosModel);

}

void EmitDataForm::setupForm(const QString &numero, int vuosi, int kuukausi, const QList<RastiData> &rastit, QVariant luettuEmitId)
{
    m_luettuEmitId = luettuEmitId;

    ui->suljeTallentamattaButton->hide();

    tarkistaKoodi99(rastit);

    QSqlDatabase::database().transaction();

    m_emitDataModel = new EmitDataModel(this, numero, vuosi, kuukausi, rastit);

    ui->emitDataView->setModel(m_emitDataModel);

    ui->emitDataView->expandAll();

    tarkistaEmit();
    tarkistaTulos();
    lataaLuettuEmit();

    valitseKilpailija();

    sqlTila();
    sqlSarja();

    valitseSarja();

    asetaAika();

    QSqlDatabase::database().commit();

}

void EmitDataForm::setupForm(const QVariant &tulosId)
{
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
                "  k.nimi AS kilpailija\n"
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

    m_emitDataModel = new EmitDataModel(
                this,
                r.value("emit").toString(),
                r.value("vuosi").toInt(),
                r.value("kuukausi").toInt(),
                RastiData::luettuRasit(r.value("luettu_emit")),
                0
    );

    ui->emitDataView->setModel(m_emitDataModel);

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

    asetaAika();

    QSqlDatabase::database().commit();
}


EmitDataForm::~EmitDataForm()
{
    delete ui;
}

void EmitDataForm::sqlTila()
{
    QSqlQuery query;

    query.prepare("SELECT id, nimi FROM tulos_tila");

    SQL_EXEC(query, );

    m_tilaModel->setQuery(query);

    ui->tilaBox->setModelColumn(1);
}

void EmitDataForm::sqlSarja()
{
    QSqlQuery query;

    query.prepare("SELECT id, nimi FROM sarja WHERE tapahtuma = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query, );

    m_sarjaModel->setQuery(query);

    ui->sarjaBox->setModelColumn(1);
}

void EmitDataForm::valitseSarja()
{
    QList<Sarja*> sarjat = Sarja::haeSarjat(this);

    // Sarjoja ei ole asetettu
    if (sarjat.count() == 0) {
        return;
    }

    int suurin = 0;
    int suurin_oikeinHaetut = 0;
    int suurin_paino = 0;

    QList<RastiData> haettu = m_emitDataModel->getRastit();

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
            suurin_oikeinHaetut = oikeinHaetut;
            suurin = sarja_i;
        }

        sarja_i++;
    }

    ui->sarjaBox->setCurrentIndex(suurin);

    Sarja *s = sarjat.at(suurin);

    // Puhdas suoritus -> hyväksytään.
    if (s->getRastit().count() == suurin_oikeinHaetut) {
        ui->tilaWidget->setStyleSheet(_("QWidget { background-color: green }"));
        ui->tilaBox->setCurrentIndex(1);
        return;
    }


    int rasti_i = 0;
    int virheita = 0;

    // Tarkistetaan pääsikö kilpailija maalliin
    foreach (RastiData d, haettu) {
        bool ok = false;

        for (int i = 0; i < virheita && rasti_i + i < s->getRastit().count(); i++) {
            Rasti r = s->getRastit().at(rasti_i + i);

            if (r.sisaltaa(d.m_rasti)) {
                ok = true;
                break;
            }
        }

        if (ok) {
            rasti_i++;
        } else {
            virheita++;
        }
    }

    // Päästiin maaliin
    if (rasti_i == s->getRastit().count()) {
        ui->tilaWidget->setStyleSheet(_("QWidget { background-color: green }"));
        ui->tilaBox->setCurrentIndex(1);
        return;
    }

    // Tarkistetaan löytyikö maalirastia, jos ei niin keskeytys
    foreach (RastiData d, haettu) {
        if (d.m_rasti == Rasti::maaliKoodi()) {
            ui->tilaWidget->setStyleSheet(_("QWidget { background-color: red }"));
            ui->tilaBox->setCurrentIndex(2);
            return;
        }
    }

    // Keskeyttänyt
    ui->tilaWidget->setStyleSheet(_("QWidget { background-color: red }"));
    ui->tilaBox->setCurrentIndex(3);
}


void EmitDataForm::tarkistaKoodi99(const QList<RastiData> &rastit)
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

void EmitDataForm::tarkistaEmit()
{
    QSqlQuery query;

    query.prepare("SELECT * FROM emit WHERE id = ?");

    query.addBindValue(m_emitDataModel->getNumero());

    SQL_EXEC(query,);

    if (query.next()) {
        return;
    }

    query.prepare("INSERT INTO emit (id, vuosi, kuukausi) VALUES (?, ?, ?)");

    query.addBindValue(m_emitDataModel->getNumero());
    query.addBindValue(m_emitDataModel->getVuosi());
    query.addBindValue(m_emitDataModel->getKuukausi());

    SQL_EXEC(query,);
}

void EmitDataForm::valitseKilpailija()
{
    QSqlQuery query;

    query.prepare("SELECT k.nimi FROM kilpailija AS k JOIN emit ON emit.kilpailija = k.id WHERE emit.id = ?");

    query.addBindValue(m_emitDataModel->getNumero());

    SQL_EXEC(query,);

    if (query.next()) {
        QSqlRecord r = query.record();

        ui->kilpailijaEdit->setText(r.value("nimi").toString());
    }
}

void EmitDataForm::asetaAika()
{
    int aika = 0;

    foreach (RastiData d, m_emitDataModel->getRastit()) {
        if (d.m_rasti == 0) {
            continue;
        }

        if (d.m_rasti == Rasti::maaliKoodi()) {
            aika = d.m_aika;
        }
    }

    if (aika == 0) {
        // FIXME jokin muu tapa tähän.
        //INFO(this, _("aikaa ei voida määrittää"));
    }

    ui->aikaTimeEdit->setTime(QTime(0,0).addSecs(aika));
}


void EmitDataForm::on_closeButton_clicked()
{
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
    query.prepare("UPDATE emit SET kilpailija = ? WHERE id = ?");

    query.addBindValue(kilpailijaId);
    query.addBindValue(m_emitDataModel->getNumero());

    SQL_EXEC(query,);

    if (m_tulosId.isNull()) {
        // Luodaan tulos
        query.prepare("INSERT INTO tulos (tapahtuma, emit, kilpailija, sarja, tila, aika) VALUES (?, ?, ?, ?, ?, ?)");

        query.addBindValue(Tapahtuma::tapahtuma()->id());
        query.addBindValue(m_emitDataModel->getNumero());
        query.addBindValue(kilpailijaId);
        query.addBindValue(sarjaId);
        query.addBindValue(tilaId);
        query.addBindValue(ui->aikaTimeEdit->time());

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
        query.addBindValue(m_emitDataModel->getNumero());
        query.addBindValue(kilpailijaId);
        query.addBindValue(sarjaId);
        query.addBindValue(tilaId);
        query.addBindValue(ui->aikaTimeEdit->time());
        query.addBindValue(m_tulosId);

        SQL_EXEC(query,);
    }

    query.prepare("INSERT INTO valiaika (tulos, numero, koodi, aika) VALUES (?, ?, ?, ?)");

    int valiaika_i = 1;

    foreach (RastiData d, m_emitDataModel->getRastit()) {
        if (d.m_rasti == 0) {
            continue;
        }

        query.addBindValue(m_tulosId);
        query.addBindValue(valiaika_i);
        query.addBindValue(d.m_rasti);
        query.addBindValue(QTime(0,0).addSecs(d.m_aika));

        SQL_EXEC(query,);

        valiaika_i++;
    }

    query.prepare("UPDATE luettu_emit SET tulos = ? WHERE id = ?");

    query.addBindValue(m_tulosId);
    query.addBindValue(m_luettuEmitId);

    SQL_EXEC(query,);

    QSqlDatabase::database().commit();

    emit requestClose(this);
}

QVariant EmitDataForm::getSarja()
{
    return m_sarjaModel->index(ui->sarjaBox->currentIndex(), 0).data(Qt::EditRole);
}

QVariant EmitDataForm::getTila()
{
    return m_tilaModel->index(ui->tilaBox->currentIndex(), 0).data(Qt::EditRole);
}

void EmitDataForm::lataaLuettuEmit()
{
    if (!m_luettuEmitId.isNull()) {
        return;
    }

    QSqlQuery query;

    query.prepare("INSERT INTO luettu_emit (tapahtuma, emit, luettu) VALUES (?, ?, ?)");

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(m_emitDataModel->getNumero());
    query.addBindValue(QDateTime::currentDateTime());

    SQL_EXEC(query,);

    m_luettuEmitId = query.lastInsertId();

    query.prepare("INSERT INTO luettu_emit_rasti (luettu_emit, numero, koodi, aika) VALUES (?, ?, ?, ?)");

    int nro = 1;
    foreach (RastiData d, m_emitDataModel->getRastit()) {
        query.addBindValue(m_luettuEmitId);
        query.addBindValue(nro);
        query.addBindValue(d.m_rasti);
        query.addBindValue(d.m_aika);

        SQL_EXEC(query,);

        nro++;
    }
}

void EmitDataForm::tarkistaTulos()
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
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(m_emitDataModel->getNumero());

    SQL_EXEC(query,);

    m_tulosModel->setQuery(query);

    if (m_tulosModel->rowCount() == 0) {
        naytaTulos();

        emit tulosLisatty();

        return;
    }

    ui->stackedWidget->setCurrentIndex(1);
}

void EmitDataForm::on_uusiButton_clicked()
{
    naytaTulos();

    ui->kilpailijaEdit->selectAll();

    emit tulosLisatty();
}

void EmitDataForm::on_korvaaButton_clicked()
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
    //ui->kilpailijaEdit->setFocus();
}

void EmitDataForm::on_sarjaBox_currentIndexChanged(int index)
{
    m_emitDataModel->setSarja(Sarja::haeSarja(m_emitDataModel, m_sarjaModel->index(index, 0).data(Qt::EditRole)));
    ui->emitDataView->expandAll();
}

void EmitDataForm::on_suljeTallentamattaButton_clicked()
{
    emit requestClose(this);
}

void EmitDataForm::naytaTulos()
{
    ui->stackedWidget->setCurrentIndex(0);

    QShortcut *s = 0;

    s = new QShortcut(QKeySequence("Alt+ENTER"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(on_closeButton_clicked()));

    s = new QShortcut(QKeySequence("Alt+RETURN"), this);
    connect(s, SIGNAL(activated()),
            this, SLOT(on_closeButton_clicked()));
}



void EmitDataForm::on_tuloksetButton_clicked()
{
    emit requestOpenTulokset();
}
