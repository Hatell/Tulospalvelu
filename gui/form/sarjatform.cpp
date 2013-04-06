#include "sarjatform.h"
#include "ui_sarjatform.h"

SarjatForm::SarjatForm(QWidget *parent) :
    UtilForm(parent),
    ui(new Ui::SarjatForm),
    m_sarjaId(),
    m_sarjaModel(new RataModel(this, Tapahtuma::tapahtuma())),
    m_rastiHeaderModel(new RastiModel(this)),
    m_headerView(new QHeaderView(Qt::Horizontal, this))
{
    ui->setupUi(this);

    ui->sarjaView->setModel(qobject_cast<RataModel*>(m_sarjaModel));
    ui->sarjaView->hideColumn(0);
    m_headerView->setModel(m_rastiHeaderModel);
    m_headerView->setStretchLastSection(true);

    ui->rastiView->setHeader(m_headerView);
    ui->rastiView->hideColumn(0);
}

SarjatForm::~SarjatForm()
{
    delete ui;
}

void SarjatForm::on_sarjaView_clicked(const QModelIndex &index)
{
    ui->rastiView->setModel(m_sarjaModel);
    m_headerView->setModel(m_rastiHeaderModel);
    ui->rastiView->setRootIndex(m_sarjaModel->index(index.row(), 0));
    m_sarjaId = m_sarjaModel->index(index.row(), 0).data(Qt::EditRole);
}

void SarjatForm::on_closeButton_clicked()
{
    emit requestClose(this);
}

void SarjatForm::on_sarjaLisaaButton_clicked()
{
    m_sarjaModel->insertRow(m_sarjaModel->rowCount(), QModelIndex());
}

void SarjatForm::on_sarjaPoistaButton_clicked()
{
    if (ui->sarjaView->currentIndex().isValid()) {
        m_sarjaModel->removeRow(ui->sarjaView->currentIndex().row(), QModelIndex());
    }
}

void SarjatForm::on_rastiLisaaButton_clicked()
{
    QModelIndex parent = ui->rastiView->rootIndex();

    if (parent.isValid()) {
        m_sarjaModel->insertRow(m_sarjaModel->rowCount(parent), parent);
    }
}

void SarjatForm::on_rastiPoistaButton_clicked()
{
    QModelIndex parent = ui->rastiView->rootIndex();

    if (parent.isValid()) {
        foreach (QModelIndex index, ui->rastiView->selectionModel()->selectedRows(0)) {
            m_sarjaModel->removeRow(index.row(), parent);
        }
    }
}
