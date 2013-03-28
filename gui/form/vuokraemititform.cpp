#include "vuokraemititform.h"
#include "ui_vuokraemititform.h"

VuokraEmititForm::VuokraEmititForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VuokraEmititForm),
    m_emitModel(new QSqlTableModel(this))
{
    ui->setupUi(this);

    ui->emitView->setModel(m_emitModel);

    sqlEmit();
}

VuokraEmititForm::~VuokraEmititForm()
{
    delete ui;
}

void VuokraEmititForm::sqlEmit()
{
    m_emitModel->setTable("emit");

    if (!ui->naytaKaikkiBox->isChecked()) {
        m_emitModel->setFilter("laina");
    } else {
        //m_emitModel->setFilter();
    }

    m_emitModel->select();
}

void VuokraEmititForm::on_naytaKaikkiBox_clicked()
{
    sqlEmit();
}

void VuokraEmititForm::on_lisaaButton_clicked()
{
    lisaaLainaEmit();
}

void VuokraEmititForm::lisaaLainaEmit()
{
    QSqlDatabase::database().transaction();

    QSqlQuery query;

    query.prepare("SELECT COUNT(*) FROM emit WHERE id = ?");

    query.addBindValue(ui->numeroBox->value());

    SQL_EXEC(query,);

    query.next();

    if (query.value(0).toInt() == 0) {
        query.prepare(
                    "INSERT INTO emit (\n"
                    "  id,\n"
                    "  vuosi,\n"
                    "  kuukausi,\n"
                    "  laina,\n"
                    "  kilpailija\n"
                    ") VALUES (\n"
                    "  ?,\n"
                    "  ?,\n"
                    "  ?,\n"
                    "  ?,\n"
                    "  ?\n"
                    ")\n"
        );

        query.addBindValue(ui->numeroBox->value());
        query.addBindValue(ui->vuosiBox->value());
        query.addBindValue(ui->kuukausiBox->value());
        query.addBindValue(1);
        query.addBindValue(QVariant());
    } else {
        query.prepare(
                    "UPDATE emit SET\n"
                    "  laina = ?\n"
                    "WHERE id = ?\n"
        );

        query.addBindValue(1);
        query.addBindValue(ui->numeroBox->value());
    }

    SQL_EXEC(query,);

    m_emitModel->select();

    QSqlDatabase::database().commit();
}

void VuokraEmititForm::on_poistaButton_clicked()
{
    QSqlDatabase::database().transaction();

    foreach (QModelIndex index, ui->emitView->selectionModel()->selectedRows(0)) {
        QSqlQuery query;

        query.prepare("DELETE FROM emit WHERE id = ?\n");

        query.addBindValue(index.data(Qt::EditRole));

        SQL_EXEC(query,);
    }

    m_emitModel->select();

    QSqlDatabase::database().commit();
}

void VuokraEmititForm::on_closeButton_clicked()
{
    close();
}
