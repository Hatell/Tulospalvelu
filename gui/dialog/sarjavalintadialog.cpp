#include "sarjavalintadialog.h"
#include "ui_sarjavalintadialog.h"

SarjaValintaDialog::SarjaValintaDialog(QWidget *parent, QList<Sarja *> sarjat) :
    QDialog(parent),
    ui(new Ui::SarjaValintaDialog),
    m_sarjat(sarjat)
{
    ui->setupUi(this);

    QStringList nimet;

    foreach (const Sarja* s, m_sarjat) {
        nimet << s->getNimi();
    }

    ui->sarjaBox->setModel(
                new QStringListModel(nimet, this)
    );
}

SarjaValintaDialog::~SarjaValintaDialog()
{
    delete ui;
}

Sarja * SarjaValintaDialog::getSarja() const
{
    return m_sarjat.at(ui->sarjaBox->currentIndex());
}
