#ifndef TESTEMITREADERWIDGET_H
#define TESTEMITREADERWIDGET_H

#include <QtWidgets>

#include "model/emitdatamodel.h"

namespace Ui {
    class TestEmitReaderWidget;
}

class TestEmitReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestEmitReaderWidget(QWidget *parent = 0);
    ~TestEmitReaderWidget();

signals:
    void readEmit(QDateTime lukuaika, QString numero, int vuosi, int kuukausi, QList<RastiData> rastit);

private slots:
    void on_lueButton_clicked();

private:
    Ui::TestEmitReaderWidget *ui;

    EmitDataModel *m_emitDataModel;
    QDataWidgetMapper *m_mapper;
};

#endif // TESTEMITREADERWIDGET_H
