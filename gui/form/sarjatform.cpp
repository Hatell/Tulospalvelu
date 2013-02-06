#include "sarjatform.h"
#include "ui_sarjatform.h"

SarjatForm::SarjatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SarjatForm),
    m_sarjaId(),
    m_sarjaModel(new QSqlTableModel(this)),
    m_rastiModel(new QSqlTableModel(this))

{
    ui->setupUi(this);

    ui->sarjaView->setModel(m_sarjaModel);
    ui->rastiView->setModel(m_rastiModel);

    sqlSarja();
}

SarjatForm::~SarjatForm()
{
    delete ui;
}

void SarjatForm::on_sarjaView_clicked(const QModelIndex &index)
{
    m_sarjaId = m_sarjaModel->index(index.row(), 0).data(Qt::EditRole);

    sqlRasti();
}

void SarjatForm::sqlSarja()
{
    m_sarjaModel->setTable("sarja");
    m_sarjaModel->setFilter(_("tapahtuma = %1").arg(QString::number(Tapahtuma::tapahtuma()->id())));
    m_sarjaModel->select();

    ui->sarjaView->hideColumn(0);
    ui->sarjaView->hideColumn(2);
}

void SarjatForm::sqlRasti()
{
    if (m_sarjaId.isNull()) {
        return;
    }

    m_rastiModel->setTable("rasti");
    m_rastiModel->setFilter(_("sarja = %1").arg(m_sarjaId.toString()));
    m_rastiModel->select();

    ui->rastiView->hideColumn(0);
    ui->rastiView->hideColumn(1);
}

void SarjatForm::on_closeButton_clicked()
{
    emit requestClose(this);
}

void SarjatForm::on_sarjaLisaaButton_clicked()
{
    QSqlQuery query;

    query.prepare("INSERT INTO sarja (tapahtuma, nimi) VALUES (?, ?)");

    query.addBindValue(Tapahtuma::tapahtuma()->id());
    query.addBindValue("UUSI SARJA");

    SQL_EXEC(query,);

    m_sarjaId = QVariant();

    m_rastiModel->clear();

    sqlSarja();
}

void SarjatForm::on_sarjaPoistaButton_clicked()
{
    if (ui->sarjaView->currentIndex().isValid()) {
        m_sarjaModel->removeRow(ui->sarjaView->currentIndex().row());
    }
}

void SarjatForm::on_rastiLisaaButton_clicked()
{
    if (m_sarjaId.isNull()) {
        return;
    }

    QSqlQuery query;

    query.prepare("INSERT INTO rasti (sarja, numero, koodi) VALUES  (?, ?, ?)");

    query.addBindValue(m_sarjaId);
    query.addBindValue(m_rastiModel->rowCount() + 1);
    query.addBindValue(0);

    SQL_EXEC(query,);

    sqlRasti();
}

void SarjatForm::on_rastiPoistaButton_clicked()
{
    if (ui->rastiView->currentIndex().isValid()) {
        m_rastiModel->removeRow(ui->rastiView->currentIndex().row());
    }
}
