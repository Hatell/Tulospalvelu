#include "serialemitreaderwidget.h"
#include "ui_serialemitreaderwidget.h"

SerialEmitReaderWidget::SerialEmitReaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialEmitReaderWidget),
    m_pollTimer(new QTimer(this)),
    m_viimeisinEmit()
  #ifdef USE_SERIAL
  ,
    m_serialPortInfos(SerialPortInfo::availablePorts())
  #endif
{
    ui->setupUi(this);

    connect(m_pollTimer, SIGNAL(timeout()),
            this, SLOT(readSerial()));

#ifdef USE_SERIAL
    foreach (SerialPortInfo info, m_serialPortInfos) {
        ui->porttiBox->addItem(info.systemLocation());
    }

    ui->kirjoitusPorttiBox->addItem(_("Ei käytössä"));

    foreach (SerialPortInfo info, m_serialPortInfos) {
        ui->kirjoitusPorttiBox->addItem(info.systemLocation());
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

    emit statusChanged("Serial: - ");

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

    m_serialPort.setPort(m_serialPortInfos.at(ui->porttiBox->currentIndex()));

    if (m_serialPort.open(QIODevice::ReadWrite)) {
        m_serialPort.setRate(SerialPort::Rate9600);
        m_serialPort.setDataBits(SerialPort::Data8);
        m_serialPort.setParity(SerialPort::NoParity);
        m_serialPort.setFlowControl(SerialPort::NoFlowControl);
        m_serialPort.setStopBits(SerialPort::OneStop);

        m_serialPort.flush();
        emit statusChanged(_("Serial: OK"));
        ui->connectButton->setText(_("Katkaise"));
        m_pollTimer->start(500);
    } else {
        emit statusChanged(_("Serial: VIRHE"));

        INFO(this, m_serialPort.errorString());
        return;
    }

//    ui->textEdit->appendPlainText(QString(m_serialPort.read(5)));

    if (m_writePort.isOpen()) {
        m_writePort.close();
    }

    ui->porttiBox->setEnabled(false);
    ui->kirjoitusPorttiBox->setEnabled(false);

    if (ui->kirjoitusPorttiBox->currentIndex() == 0) {
        return;
    }

    //qDebug() << _("Avataan kirjoitus");

    m_writePort.setPort(m_serialPortInfos.at(ui->kirjoitusPorttiBox->currentIndex() - 1));

    if (m_writePort.open(QIODevice::ReadWrite)) {
        m_writePort.setRate(SerialPort::Rate9600);
        m_writePort.setDataBits(SerialPort::Data8);
        m_writePort.setParity(SerialPort::NoParity);
        m_writePort.setFlowControl(SerialPort::NoFlowControl);
        m_writePort.setStopBits(SerialPort::OneStop);

        m_writePort.flush();

        //qDebug() << _("Onnistui");
    }
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
            emit readEmit(numero, vuosi, kuukausi, rastit);

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
#ifdef USE_SERIAL
    if (m_serialPort.isOpen()) {
        m_serialPort.close();
    }
#endif
}