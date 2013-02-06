#include "tapahtumadialog.h"
#include "ui_tapahtumadialog.h"

TapahtumaDialog::TapahtumaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TapahtumaDialog),
    m_tapahtumaModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    ui->tapahtumaView->setModel(m_tapahtumaModel);

    ui->cancelButton->setEnabled(Tapahtuma::tapahtuma()->id() != 0);

    sqlTapahtuma();
}

TapahtumaDialog::~TapahtumaDialog()
{
    delete ui;
}

void TapahtumaDialog::on_valitseButton_clicked()
{
    foreach (QModelIndex index, ui->tapahtumaView->selectionModel()->selectedRows(0)) {
        Tapahtuma::valitseTapahtuma(index.data(Qt::EditRole).toInt());

        accept();

        return;
    }

    INFO(this, _("Valitse tapahtuma listasta."));
}

void TapahtumaDialog::sqlTapahtuma()
{
    QSqlQuery query;

    query.prepare("SELECT id, nimi FROM tapahtuma ORDER BY id DESC");

    SQL_EXEC(query,);

    m_tapahtumaModel->setQuery(query);
}


void TapahtumaDialog::on_uusiButton_clicked()
{
    QString nimi = QInputDialog::getText(this, _("Tulospalvelu"),
                                         _("Anna tapahtuman nimi:"));

    if (nimi.isEmpty()) {
        return;
    }

    Tapahtuma::luoUusiTapahtuma(nimi);

    accept();
}
