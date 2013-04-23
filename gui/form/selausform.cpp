#include "selausform.h"
#include "ui_selausform.h"

SelausForm::SelausForm(QWidget *parent) :
    UtilForm(parent),
    ui(new Ui::SelausForm),
    m_tulosModel(new QSqlQueryModel(this)),
    m_currentRow(-1)
{
    ui->setupUi(this);

    connect(ui->tulosForm, SIGNAL(requestClose(QWidget*)),
            this, SLOT(handleRequestClose()));

    sqlTulos();

    on_viimeinenButton_clicked();
}

SelausForm::~SelausForm()
{
    delete ui;
}

void SelausForm::updateForm()
{
    sqlTulos();
    setupTulosForm();
}

void SelausForm::handleRequestClose()
{
    emit requestClose(this);
}

void SelausForm::on_ensimmainenButton_clicked()
{
    if (m_tulosModel->rowCount() == 0) {
        return;
    }

    m_currentRow = 0;

    setupTulosForm();
}

void SelausForm::on_edellinenButton_clicked()
{
    if (m_tulosModel->rowCount() == 0) {
        return;
    }

    m_currentRow--;

    setupTulosForm();
}

void SelausForm::on_seuraavaButton_clicked()
{
    if (m_tulosModel->rowCount() == 0) {
        return;
    }

    if (m_currentRow < m_tulosModel->rowCount() - 1) {
        m_currentRow++;
    }

    setupTulosForm();
}

void SelausForm::on_viimeinenButton_clicked()
{
    if (m_tulosModel->rowCount() == 0) {
        return;
    }

    m_currentRow = m_tulosModel->rowCount() - 1;

    setupTulosForm();
}

void SelausForm::sqlTulos()
{
    QSqlQuery query;

    query.prepare("SELECT id FROM tulos WHERE tapahtuma = ?");

    query.addBindValue(Tapahtuma::tapahtuma()->id());

    SQL_EXEC(query,);

    m_tulosModel->setQuery(query);

    while (m_tulosModel->canFetchMore()) {
        m_tulosModel->fetchMore();
    }
}

void SelausForm::setupTulosForm()
{
    if (m_currentRow < 0) {
        m_currentRow = -1;
    }

    if (m_currentRow >= m_tulosModel->rowCount()) {
        m_currentRow = -1;
    }

    if (m_currentRow == -1) {
        return;
    }

    // Jos ensimmäinen tulos
    ui->edellinenButton->setDisabled(m_currentRow == 0);
    ui->ensimmainenButton->setDisabled(m_currentRow == 0);

    // Jos viimeinen tulos
    ui->seuraavaButton->setDisabled(m_currentRow + 1 == m_tulosModel->rowCount());
    ui->viimeinenButton->setDisabled(m_currentRow + 1 == m_tulosModel->rowCount());

    QVariant id = m_tulosModel->index(m_currentRow, 0).data(Qt::EditRole);

    ui->headerLabel->setText(
        _("Tulos (%1/%2):")
            .arg(QString::number(m_currentRow + 1))
            .arg(QString::number(m_tulosModel->rowCount()))
    );
    ui->idLabel->setText(
                _("ID: %1")
                .arg(id.toString())
    );
    ui->tulosForm->setupForm(id);
}
