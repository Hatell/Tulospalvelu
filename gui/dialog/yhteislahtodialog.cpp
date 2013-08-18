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
    QSqlQuery valiaikaEkaQuery;
    QSqlQuery updateValiaika;

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
    );

    valiaikaEkaQuery.prepare(
                "SELECT\n"
                " aika\n"
                "FROM valiaika\n"
                "WHERE tulos = ?\n"
                "  AND numero = 1\n"
    );

    updateValiaika.prepare(
                "UPDATE valiaika SET\n"
                "  aika = ?\n"
                "WHERE id = ?\n"
    );

    SQL_EXEC(query, false);

    qDebug() << lahtoaika;

    while (query.next()) {
        QSqlRecord r = query.record();

        updateTulos.addBindValue(QTime(0, 0).addSecs(lahtoaika.secsTo(r.value("maaliaika").toDateTime())));
        updateTulos.addBindValue(r.value("id"));

        SQL_EXEC(updateTulos, false);

        // väliajat
        valiaikaEkaQuery.addBindValue(r.value("id"));

        SQL_EXEC(valiaikaEkaQuery, false);

        if (!valiaikaEkaQuery.next()) {
            continue;
        }

        int korjaus = valiaikaEkaQuery.value(0).toTime().secsTo(QTime(0, 0));

        valiaikaQuery.addBindValue(r.value("id"));

        SQL_EXEC(valiaikaQuery, false);

        while (valiaikaQuery.next()) {
            QSqlRecord vr = valiaikaQuery.record();

            updateValiaika.addBindValue(vr.value("aika").toTime().addSecs(korjaus));
            updateValiaika.addBindValue(vr.value("id"));

            SQL_EXEC(updateValiaika, false);
        }
    }

    QSqlDatabase::database().commit();

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
