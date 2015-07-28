#ifndef SERIALEMITREADERWIDGET_H
#define SERIALEMITREADERWIDGET_H

#include <QtWidgets>

#ifdef USE_SERIAL
#include <QtSerialPort/QtSerialPort>

QT_USE_NAMESPACE

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

    void openSerial(const QString& port);
    void closeSerial();

    QString getPort() const;

    QStringList getPorts() const;
signals:
    void readEmit(QDateTime lukuaika, QString numero, int vuosi, int kuukausi, QList<RastiData> rastit);
    void statusChanged(QString status);

private slots:
    void on_connectButton_clicked();

    void readSerial();

private:
    Ui::SerialEmitReaderWidget *ui;

    QSettings m_settings;

    QTimer *m_pollTimer;
    QString m_viimeisinEmit;

#ifdef USE_SERIAL
    QList<QSerialPortInfo> m_serialPortInfos;

    QSerialPort m_serialPort;
    QSerialPort m_writePort;
#endif
};

#endif // SERIALEMITREADERWIDGET_H
