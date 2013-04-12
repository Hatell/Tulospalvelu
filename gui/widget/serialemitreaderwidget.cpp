#include "serialemitreaderwidget.h"
#include "ui_serialemitreaderwidget.h"

SerialEmitReaderWidget::SerialEmitReaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialEmitReaderWidget),
    m_pollTimer(new QTimer(this)),
    m_viimeisinEmit()
  #ifdef USE_SERIAL
  ,
    m_serialPortInfos(QSerialPortInfo::availablePorts())
  #endif
{
    ui->setupUi(this);

    connect(m_pollTimer, SIGNAL(timeout()),
            this, SLOT(readSerial()));

#ifdef USE_SERIAL
    foreach (QSerialPortInfo info, m_serialPortInfos) {
        ui->porttiBox->addItem(info.portName());
    }

    ui->kirjoitusPorttiBox->addItem(_("Ei käytössä"));

    foreach (QSerialPortInfo info, m_serialPortInfos) {
        ui->kirjoitusPorttiBox->addItem(info.portName());
    }
#endif
}

SerialEmitReaderWidget::~SerialEmitReaderWidget()
{
    closeSerial();

    delete ui;
}

void SerialEmitReaderWidget::on_connectButton_clicked()
{
    if (ui->kirjoitusPorttiBox->currentIndex() - 1 == ui->porttiBox->currentIndex()) {
        QMessageBox::information(this, _("Tulospalvelu"),
                                 _("Valitse luku- ja kirjoitusportti niin, ettei ne ole sama portti."));
        return;
    }

    emit statusChanged("Lukulaite: Yhteys katkaistu");

#ifdef USE_SERIAL

    if (m_serialPort.isOpen()) {
        closeSerial();
        return;
    }

    openSerial(ui->porttiBox->currentText());

#endif
}

void SerialEmitReaderWidget::readSerial()
{
#ifdef USE_SERIAL
    bool head_ok = false;

    while (m_serialPort.bytesAvailable() >= 2 && !head_ok) {
        QByteArray head = m_serialPort.peek(2);
        quint8 *head_data = (quint8 *)head.data();

        head_ok = (head_data[0] == (0xFF ^ 0xDF) && head_data[1] == (0xFF ^ 0xDF));

        if (!head_ok) {
            qDebug() << m_serialPort.read(1).toHex();
        }
    }

    if (m_serialPort.bytesAvailable() >= 217) {
        QByteArray dataArray = m_serialPort.read(217);
        quint8 *data = (quint8 *)dataArray.data();
        quint8 tarkiste1 = 0;
        quint8 tarkiste2 = 0;

        // Kirjoitetaan data eteenpäin ennen prosessointia.
        if (m_writePort.isOpen()) {
            m_writePort.write(dataArray);
        }

        for (int i = 0; i < 217; i++) {
            data[i] ^= 0xDF;
        }

        // lasketaan tavujen 3 - 10 tarkiste
        for (int i = 2; i < 10; i++) {
            tarkiste1 += data[i];
        }

        // lasketaan kokodatan tarkiste
        for (int i = 0; i < 217; i++) {
            tarkiste2 += data[i];
        }

        QString numero = QString::number(((data[4] << 16) + (data[3] << 8) + data[2]));

        int kuukausi = data[6];
        int vuosi = data[7];

        QList<RastiData> rastit;

        for (int i = 10; i < 160; i = i + 3) {
            rastit.append(RastiData(data[i], (data[i+2] << 8) + data[i+1]));
        }

        if (m_viimeisinEmit != numero && !tarkiste1 && !tarkiste2) {
            emit readEmit(QDateTime::currentDateTime(), numero, vuosi, kuukausi, rastit);

            m_viimeisinEmit = numero;
        }

        ui->textEdit->appendPlainText(
                    _("[%1] Luettu emitti: %2 %3 %4")
                    .arg(QTime::currentTime().toString("HH.mm.ss"))
                    .arg(numero)
                    .arg(tarkiste1 ? _("T1(%1)").arg(QString::number(tarkiste1)) : "")
                    .arg(tarkiste2 ? _("T2(%1)").arg(QString::number(tarkiste2)) : "")
        );
    }
#endif
}

void SerialEmitReaderWidget::closeSerial()
{
    emit statusChanged(_("Lukulaite: Yhteys katkaistu"));

#ifdef USE_SERIAL

    if (m_serialPort.isOpen()) {
        m_serialPort.close();
        ui->connectButton->setText(_("Yhdistä"));
        m_pollTimer->stop();

        if (m_writePort.isOpen()) {
            m_writePort.close();
        }

        ui->porttiBox->setEnabled(true);
        ui->kirjoitusPorttiBox->setEnabled(true);

        return;
    }

#endif
}

QStringList SerialEmitReaderWidget::getPorts() const
{
    QStringList ports;

#ifdef USE_SERIAL

    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        ports << info.portName();
    }

#endif

    return ports;
}

void SerialEmitReaderWidget::openSerial(const QString &port)
{
#ifdef USE_SERIAL

    bool found = false;
    QSerialPortInfo info;

    foreach (info, QSerialPortInfo::availablePorts()) {
        if (port == info.portName()) {
            found = true;
            break;
        }
    }

    if (!found) {
        return;
    }

    closeSerial();

    qDebug() << "openSerial" << port;

    //m_serialPort.setPort(m_serialPortInfos.at(ui->porttiBox->currentIndex()));
    m_serialPort.setPort(info);

    if (m_serialPort.open(QIODevice::ReadWrite)) {
        m_serialPort.setBaudRate(QSerialPort::Baud9600);
        m_serialPort.setDataBits(QSerialPort::Data8);
        m_serialPort.setParity(QSerialPort::NoParity);
        m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
        m_serialPort.setStopBits(QSerialPort::OneStop);

        m_serialPort.flush();
        emit statusChanged(_("Lukulaite: <span style=\"color: green;\">Yhdistetty</span>"));
        ui->connectButton->setText(_("Katkaise"));
        m_pollTimer->start(500);
    } else {
        emit statusChanged(_("Lukulaite: <span style=\"color: red;\">VIRHETILANNE</span>"));

        INFO(this, m_serialPort.errorString());
        return;
    }

    if (m_writePort.isOpen()) {
        m_writePort.close();
    }

    ui->porttiBox->setEnabled(false);
    ui->kirjoitusPorttiBox->setEnabled(false);

    if (ui->kirjoitusPorttiBox->currentIndex() == 0) {
        return;
    }

    m_writePort.setPort(m_serialPortInfos.at(ui->kirjoitusPorttiBox->currentIndex() - 1));

    if (m_writePort.open(QIODevice::ReadWrite)) {
        m_writePort.setBaudRate(QSerialPort::Baud9600);
        m_writePort.setDataBits(QSerialPort::Data8);
        m_writePort.setParity(QSerialPort::NoParity);
        m_writePort.setFlowControl(QSerialPort::NoFlowControl);
        m_writePort.setStopBits(QSerialPort::OneStop);

        m_writePort.flush();
    }

#endif
}

QString SerialEmitReaderWidget::getPort() const
{
    QString port;

#ifdef USE_SERIAL

    if (m_serialPort.isOpen()) {
        port = m_serialPort.portName();
    }

#endif

    return port;
}
