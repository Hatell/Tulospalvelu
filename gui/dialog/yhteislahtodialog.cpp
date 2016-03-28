#include "yhteislahtodialog.h"
#include "ui_yhteislahtodialog.h"

YhteislahtoDialog::YhteislahtoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::YhteislahtoDialog),
    m_sarjaModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    ui->sarjaView->setModel(m_sarjaModel);

    sqlSarja();
    sqlLahtoaika();
}

YhteislahtoDialog::~YhteislahtoDialog()
{
    delete ui;
}

QDateTime YhteislahtoDialog::getLahtoaika() const
{
    return ui->lahtoDateTimeEdit->dateTime();
}

QVariant YhteislahtoDialog::getSarjaId() const
{
    foreach (QModelIndex index, ui->sarjaView->selectionModel()->selectedRows(0)) {
        return index.data(Qt::EditRole);
    }

    return QVariant();
}

bool YhteislahtoDialog::muutaYhteislahdoksi(const QVariant &sarja_id, const QDateTime &lahtoaika)
{
    QSqlDatabase::database().transaction();

    QSqlQuery query;
    QSqlQuery updateTulos;
    QSqlQuery valiaikaQuery;
    QSqlQuery updateValiaika;

    QList< UusiTulos > tulokset;

    Sarja *sarja = Sarja::haeSarja(0, sarja_id);

    query.prepare(
                "SELECT\n"
                "  id,\n"
                "  maaliaika\n"
                "FROM tulos\n"
                "WHERE tapahtuma = ?\n"
                "  AND sarja = ?\n"
                "  AND NOT poistettu\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue(sarja_id);

    updateTulos.prepare(
                "UPDATE tulos SET\n"
                "  aika = ?\n"
                "WHERE id = ?\n"
    );

    valiaikaQuery.prepare(
                "SELECT\n"
                " *\n"
                "FROM valiaika\n"
                "WHERE tulos = ?\n"
                "ORDER BY numero DESC\n" // Halutaan maaliaika ensin
    );

    updateValiaika.prepare(
                "UPDATE valiaika SET\n"
                "  aika = ?\n"
                "WHERE id = ?\n"
    );

    SQL_EXEC(query, false);

    //qDebug() << lahtoaika;

    while (query.next()) {
        QTime maalitulos;
        QSqlRecord r = query.record();

        updateTulos.addBindValue(QTime(0, 0).addSecs(lahtoaika.secsTo(r.value("maaliaika").toDateTime())));
        updateTulos.addBindValue(r.value("id"));

        SQL_EXEC(updateTulos, false);

        // väliajat
        valiaikaQuery.addBindValue(r.value("id"));

        SQL_EXEC(valiaikaQuery, false);

        while (valiaikaQuery.next()) {
            QSqlRecord vr = valiaikaQuery.record();

            if (maalitulos.isNull()
                && sarja->getMaalirasti().sisaltaa(vr.value("koodi").toInt())) {
                maalitulos = vr.value("aika").toTime();
            }

            QTime aika = vr.value("aika").toTime();

            int delta = maalitulos.secsTo(aika);

            QDateTime lukuaika = r.value("maaliaika").toDateTime().addSecs(delta);
            QTime tulos = QTime(0,0);

            if (lahtoaika <= lukuaika) {
                tulos = QTime(0,0).addSecs(lahtoaika.secsTo(lukuaika));
            }

            /*qDebug() << "tulos:" << r.value("id").toInt() << vr.value("id").toInt();
            qDebug() << "maaliaika:" << r.value("maaliaika").toDateTime();
            qDebug() << "aika:" << aika;
            qDebug() << "delta:" << delta;
            qDebug() << "lahtoaika:" << lahtoaika;
            qDebug() << "lukuaika:" << lukuaika;
            qDebug() << "tulos:" << tulos;*/

            // Bugi SQLite:ssä tai Qt:n SQLite toteutuksessa. Ei voi päivittää väliaikaa tässä kohtaa.
            tulokset.append(UusiTulos(vr.value("id"), tulos));
        }
    }

    foreach (UusiTulos up, tulokset) {
        updateValiaika.addBindValue(up.aika);
        updateValiaika.addBindValue(up.valiaika);

        SQL_EXEC(updateValiaika, false);
    }

    QSqlDatabase::database().commit();

    delete sarja;
    return true;
}

void YhteislahtoDialog::sqlSarja()
{
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  id,\n"
                "  nimi\n"
                "FROM sarja\n"
                "WHERE tapahtuma = ?\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    m_sarjaModel->setQuery(query);

    ui->sarjaView->selectionModel()->setCurrentIndex(ui->sarjaView->model()->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void YhteislahtoDialog::sqlLahtoaika()
{
    QSqlQuery query;

    query.prepare(
                "SELECT\n"
                "  maaliaika\n"
                "FROM tulos\n"
                "WHERE tapahtuma = ?\n"
                "LIMIT 1\n"
    );

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    QDateTime lahtoaika = QDateTime::currentDateTime();

    if (query.next()) {
        lahtoaika = query.value(0).toDateTime();
    }

    ui->lahtoDateTimeEdit->setDateTime(lahtoaika);
}
