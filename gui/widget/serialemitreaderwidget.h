#ifndef SERIALEMITREADERWIDGET_H
#define SERIALEMITREADERWIDGET_H

#include <QtGui>

#ifdef USE_SERIAL

#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>

QT_USE_NAMESPACE_SERIALPORT

#endif

#include "type/rastidata.h"

#include "makrot.h"

namespace Ui {
    class SerialEmitReaderWidget;
}


class SerialEmitReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialEmitReaderWidget(QWidget *parent = 0);
    ~SerialEmitReaderWidget();

    void closeSerial();

signals:
    void readEmit(QString numero, int vuosi, int kuukausi, QList<RastiData> rastit);
    void statusChanged(QString status);

private slots:
    void on_connectButton_clicked();

    void readSerial();

private:
    Ui::SerialEmitReaderWidget *ui;

    QTimer *m_pollTimer;
    QString m_viimeisinEmit;

#ifdef USE_SERIAL
    QList<SerialPortInfo> m_serialPortInfos;

    SerialPort m_serialPort;
    SerialPort m_writePort;
#endif
};

#endif // SERIALEMITREADERWIDGET_H
