#include "testemitreaderwidget.h"
#include "ui_testemitreaderwidget.h"

TestEmitReaderWidget::TestEmitReaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestEmitReaderWidget),
    m_emitDataModel(new EmitDataModel(this)),
    m_mapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);

    ui->rastiView->setModel(m_emitDataModel);
    ui->rastiView->setRootIndex(m_emitDataModel->index(0, 0));

    m_mapper->setModel(m_emitDataModel);
    m_mapper->setRootIndex(QModelIndex());

    m_mapper->addMapping(ui->numeroEdit, 0);
    m_mapper->addMapping(ui->vuosiBox, 1);
    m_mapper->addMapping(ui->kuukausiBox, 2);

    m_mapper->toFirst();

    m_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
}

TestEmitReaderWidget::~TestEmitReaderWidget()
{
    delete ui;
}

void TestEmitReaderWidget::on_lueButton_clicked()
{
    emit readEmit(
                QDateTime::currentDateTime(),
                m_emitDataModel->getNumero(),
                m_emitDataModel->getVuosi(),
                m_emitDataModel->getKuukausi(),
                m_emitDataModel->getRastit()
    );
}
