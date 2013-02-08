#include "tuloksetform.h"
#include "ui_tuloksetform.h"

TuloksetForm::TuloksetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TuloksetForm),
    m_filterModel(new QSortFilterProxyModel(this)),
    m_tulosModel(new QSqlQueryModel(this)),
    m_sarjat(),
    m_tulokset()
{
    ui->setupUi(this);

    m_filterModel->setSourceModel(m_tulosModel);
    ui->tulosView->setModel(m_filterModel);

    on_comboBox_currentIndexChanged(0);

    on_updateButton_clicked();
}

TuloksetForm::~TuloksetForm()
{
    delete ui;
}

void TuloksetForm::sqlTulokset()
{
    QSqlQuery query;

    query.prepare("SELECT COUNT(*) AS kpl, tila FROM tulos WHERE tapahtuma = ? GROUP BY tila");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    int yht = 0;

    while (query.next()) {
        QSqlRecord r = query.record();

        switch (r.value("tila").toInt()) {
            case 1:
                ui->avoimiaLabel->setText(r.value("kpl").toString());
                break;
            case 2:
                ui->hyvaksyttyjaLabel->setText(r.value("kpl").toString());
                break;
            case 3:
                ui->hylattyjaLabel->setText(r.value("kpl").toString());
                break;
        }

        yht += r.value("kpl").toInt();
    }

    ui->tuloksiaLabel->setText(QString::number(yht));
}

void TuloksetForm::on_closeButton_clicked()
{
    emit requestClose(this);
}

void TuloksetForm::updateTulosEdit()
{
    QTextEdit *edit = ui->tulosEdit;

    edit->clear();

    m_tulosString.clear();

    foreach (Sarja* s, m_sarjat) {
        QList<Tulos> tulokset = m_tulokset.value(s->getNimi());
        QTime ekaAika = QTime();

        edit->append(_("<h3>%1</h3>").arg(s->getNimi()));
        m_tulosString += _("<h3>%1</h3>").arg(s->getNimi());

        QString tulos;

        int lahti = tulokset.count();
        int kesk = 0;

        foreach (Tulos t, tulokset) {
            if (t.m_tila != 2) {
                kesk++;
            }
        }

        tulos += _("(Lähti: %1, hylätty/keskeyttäny: %2)\n\n")
                .arg(QString::number(lahti))
                .arg(QString::number(kesk))
        ;

        tulos += _("%1 %2 %3  %4\n")
               .arg("Sija", -5)
               .arg("Kilpailija", -30)
               .arg("Tulos", -8)
               .arg("", 9)
        ;

        foreach (Tulos t, tulokset) {
            QString erotus = "";
            QString aika = "";

            if (t.m_tila == 2) {
                //aika = t.m_aika.toString("HH.mm.ss");
                aika = timeFormat(t.m_aika);

                if (ekaAika.isValid()) {
                    //erotus = QTime(0, 0).addSecs(edellinen.secsTo(t.m_aika)).toString("+HH.mm.ss");
                    erotus = "+" + timeFormat(QTime(0, 0).addSecs(ekaAika.secsTo(t.m_aika)));
                } else {
                    ekaAika = t.m_aika;
                }
            } else {
                aika = "hyl.";
            }

            tulos += _("%1 %2 %3  %4\n")
                   .arg(QString::number(t.m_sija) + ".", 5)
                   .arg(t.m_kilpailija, -30)
                   .arg(aika, 8)
                   .arg(erotus, 9);
        }

        edit->append(_("<pre>%1</pre>").arg(tulos));

        m_tulosString += _("<pre>%1</pre>").arg(tulos);
    }
}

void TuloksetForm::updateValiaikaEdit()
{
    QTextEdit *edit = ui->valiaikaEdit;

    edit->clear();

    m_valiaikaString.clear();

    foreach (Sarja* s, m_sarjat) {
        m_valiaikaString.append(createValiaika(s));
        m_valiaikaString.append(createRastivali(s));
    }

    edit->append(m_valiaikaString);
}


void TuloksetForm::updateLehteenEdit()
{
    QPlainTextEdit *edit = ui->lehteenEdit;

    edit->clear();

    foreach (Sarja* s, m_sarjat) {
        QList<Tulos> tulokset = m_tulokset.value(s->getNimi());

        QString tulos;

        int lahti = tulokset.count();
        int kesk = 0;
        int hyl = 0;

        foreach (Tulos t, tulokset) {
            switch (t.m_tila) {
            case 3:
                hyl++;
                break;
            case 4:
                kesk++;
                break;
            }
        }

        tulos += _("%1 (Lähti: %2, Keskeytti: %3, Hylätty: %4)\n")
                .arg(s->getNimi())
                .arg(QString::number(lahti))
                .arg(QString::number(kesk))
                .arg(QString::number(hyl))
        ;

        foreach (Tulos t, tulokset) {
            QString sija = "";
            QString aika = "";

            switch (t.m_tila) {
            case 2:
                aika = timeFormat(t.m_aika);
                sija = _("%1)").arg(QString::number(t.m_sija));
                break;
            case 3:
                aika = "Hyl.";
                break;
            case 4:
                aika = "Kesk.";
                break;
            }

            tulos += _("%1 %2, , %3,  ")
                   .arg(sija)
                   .arg(t.m_kilpailija)
                   .arg(aika)
            ;
        }

        edit->appendPlainText(_("%1.\n").arg(tulos));
    }

    edit->appendPlainText("");
}


void TuloksetForm::sqlTulos()
{
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  t.id,\n"
                "  t.emit,\n"
                "  tila.nimi AS tila,\n"
                "  t.aika,\n"
                "  s.nimi,\n"
                "  k.nimi\n"
                "FROM tulos AS t\n"
                "  JOIN tulos_tila AS tila ON tila.id = t.tila\n"
                "  JOIN kilpailija AS k ON k.id = t.kilpailija\n"
                "  JOIN sarja AS s ON s.id = t.sarja\n"
                "WHERE t.tapahtuma = ?\n"
                "ORDER BY t.id DESC\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    m_tulosModel->setQuery(query);
}


void TuloksetForm::on_updateButton_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QSqlDatabase::database().transaction();

    m_tulokset.clear();

    m_sarjat = Sarja::haeSarjat(this);

    foreach (Sarja* s, m_sarjat) {
        m_tulokset.insert(s->getNimi(), Tulos::haeTulokset(s));
    }

    sqlTulokset();

    // Tabit
    sqlTulos();
    updateTulosEdit();
    updateValiaikaEdit();
    updateLehteenEdit();

    QSqlDatabase::database().commit();

    QApplication::restoreOverrideCursor();
}

void TuloksetForm::on_fileButton_clicked()
{
    bool html = false;
    QString *tulos = 0;
    QString text;

    switch (ui->tabWidget->currentIndex()) {
    case 1:
        html = true;
        tulos = &m_tulosString;
        break;
    case 2:
        html = true;
        tulos = &m_valiaikaString;
        break;
    case 3:
        html = false;
        text = ui->lehteenEdit->toPlainText();
        break;
    }

    if (!tulos && text.isNull()) {
        return;
    }


    QString fn = QFileDialog::getSaveFileName(this, _("Valitse tiedosto"));

    if (fn.isNull()) {
        return;
    }

    QFile file(fn);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        INFO(this, _("Tiedostoa ei voida avata kirjoittamista varten."));
    }

    if (html) {
        file.write("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"/></head><body>\n");
        file.write(tulos->toLatin1());
        file.write("</body></html>");
    } else {
        file.write(text.replace('\n', "\r\n").toLatin1());
    }

    file.close();
}

QString TuloksetForm::createValiaika(Sarja* s)
{
    QString res;
    QList<Tulos> tulokset = m_tulokset.value(s->getNimi());

    res.append(_("<h3>%1 Tilanne rasteilla</h3>").arg(s->getNimi()));

    QString tulos = _("%1 %2")
        .arg("Sija", -5)
        .arg("Kilpailija", -30)
    ;

    QMap<int, QList<Valiaika> > rastienValiajat;

    foreach (Rasti r, s->getRastit()) {
        if (r.sisaltaa(Rasti::maaliKoodi())) {
            continue;
        }

        rastienValiajat.insert(r.getId().toInt(), Valiaika::haeRastiValiajat(r.getId()));

        tulos +=
            _(" %1")
            .arg("   " + QString::number(r.getNumero()), -13)
        ;
    }

    tulos +=
        _(" %1\n")
        .arg("Tulos", -13)
    ;

    foreach (Tulos t, tulokset) {
        if (t.m_tila != 2) {
            continue;
        }

        QString line = _("%1 %2")
                .arg(QString::number(t.m_sija) + ".", 5)
                .arg(t.m_kilpailija, -30)
        ;

        QString aika; // = t.m_aika.toString("HH.mm.ss");
        aika = timeFormat(t.m_aika);

        foreach (Rasti r, s->getRastit()) {
            if (r.sisaltaa(Rasti::maaliKoodi())) {
                continue;
            }

            foreach (Valiaika v, t.m_valiajat) {
                if (r.sisaltaa(v.m_koodi)
                    && r.getNumero() <= v.m_numero) {

                    foreach (Valiaika tmp_v, rastienValiajat.value(r.getId().toInt())) {
                        if (tmp_v.m_id == v.m_id) {
                            v = tmp_v;
                            break;
                        }
                    }

                    line += _(" %1-%2 ")
                            .arg(v.m_sija, 3)
                            .arg(timeFormat(v.m_aika), -8)
                            //.arg(v.m_aika.toString("HH.mm.ss"), 8)
                    ;

                    break;
                }
            }
        }

        line += _(" %1 %2\n")
                .arg(aika, -13)
                .arg(t.m_kilpailija)
        ;

        tulos += line;
    }

    res.append(_("<pre>%1</pre>").arg(tulos));

    return res;
}

QString TuloksetForm::createRastivali(Sarja* s)
{
    QString res;

    QList<Tulos> tulokset;
    QMap< int, QList<QTime> > rastiAjat;

    foreach (Tulos t, m_tulokset.value(s->getNimi())) {
        if (t.m_tila != 2) {
            continue;
        }

        QTime edellinen;

        QList<Valiaika> valiajat;

        foreach (Valiaika v, Valiaika::karsiYlimaaraiset(t.m_valiajat, s->getRastit())) {
            QTime tulos =  v.m_aika;

            if (!edellinen.isNull()) {
                v.m_aika = QTime(0, 0).addSecs(edellinen.secsTo(v.m_aika));
            }

            valiajat.append(v);

            edellinen = tulos;
        }

        t.m_valiajat = valiajat;
        tulokset.append(t);
    }

    // Kerätään ajat rasteittain
    foreach (Rasti r, s->getRastit()) {
        QList<QTime> ajat;

        foreach (Tulos t, tulokset) {
            foreach (Valiaika v, t.m_valiajat) {
                if (v.m_numero == r.getNumero()) {
                    ajat.append(v.m_aika);
                    break;
                }
            }
        }

        rastiAjat.insert(r.getNumero(), ajat);
    }

    res.append(_("<h3>%1 Rastivälien ajat</h3>").arg(s->getNimi()));

    QString tulos = _("%1 %2")
        .arg("Sija", -5)
        .arg("Kilpailija", -30)
    ;

    foreach (Rasti r, s->getRastit()) {
        if (r.sisaltaa(Rasti::maaliKoodi())) {
            tulos +=
                _(" %1")
                .arg("   " + QString::number(r.getNumero() - 1) + " - M", -13)
            ;
        } else {
            tulos +=
                _(" %1")
                .arg("   " + QString::number(r.getNumero() - 1) + " - " + QString::number(r.getNumero()), -13)
            ;
        }
    }

    tulos +=
        _(" %1\n")
        .arg("Tulos", -13)
    ;

    foreach (Tulos t, tulokset) {
        QString line = _("%1 %2")
                .arg(QString::number(t.m_sija) + ".", 5)
                .arg(t.m_kilpailija, -30)
        ;

        QString aika;// = t.m_aika.toString("HH.mm.ss");
        aika = timeFormat(t.m_aika);


        foreach (Valiaika v, t.m_valiajat) {
            int sija = 1;

            foreach (QTime t, rastiAjat.value(v.m_numero)) {
                if (t < v.m_aika) {
                    sija++;
                    continue;
                }
            }

            line += _(" %1-%2 ")
                    .arg(sija, 3)
                    .arg(timeFormat(v.m_aika), -8)
                    //.arg(v.m_aika.toString("HH.mm.ss"), 8)
            ;
        }

        line += _(" %1 %2\n")
                .arg(aika, -13)
                .arg(t.m_kilpailija)
        ;

        tulos += line;
    }

    res.append(_("<pre>%1</pre>").arg(tulos));

    return res;
}

QString TuloksetForm::timeFormat(const QTime &time) const
{
    return (time.toString((time.hour() ? _("H.") : _("")) + (time.hour() || time.minute() ? _("mm.") : _("")) + _("ss")));
}

void TuloksetForm::on_tulosAvaaButton_clicked()
{
    foreach (QModelIndex index, ui->tulosView->selectionModel()->selectedRows(0)) {
        emit requestTulosForm(index.data(Qt::EditRole));
    }
}

void TuloksetForm::on_lineEdit_textChanged(const QString &arg1)
{
    m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filterModel->setFilterWildcard(arg1);
}

void TuloksetForm::on_comboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        m_filterModel->setFilterKeyColumn(5);
        break;
    case 1:
        m_filterModel->setFilterKeyColumn(1);
        break;
    case 2:
        m_filterModel->setFilterKeyColumn(4);
        break;
    case 3:
        m_filterModel->setFilterKeyColumn(2);
        break;
    }
}
