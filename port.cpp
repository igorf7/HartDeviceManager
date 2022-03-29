#include "port.h"

#include <qdebug.h>

/**
 * @brief ComPort::ComPort constructor.
 * @param parent
 */
ComPort::ComPort(QObject *parent) :
    QObject(parent)
{
}

/**
 * @brief ComPort::~ComPort destructor.
 */
ComPort::~ComPort()
{
    emit quitComPort();
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief ComPort::onPortStart. Run at class start.
 */
void ComPort:: onPortStart()
{
    qDebug("Hello from comPort Thread!");
    /* Connect signals with slots */
    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

/**
 * @brief ComPort::onConnectClicked slot implementation.
 * @param port - port name
 */
void ComPort:: onConnectClicked(const QString &port) {
    if (!serialPort.isOpen()) {
        serialPort.setPortName(port);
        if (serialPort.open(QIODevice::ReadWrite)) {
            serialPort.setBaudRate(QSerialPort::Baud1200);
            serialPort.setDataBits(QSerialPort::Data8);
            serialPort.setStopBits(QSerialPort::OneStop);
            serialPort.setParity(QSerialPort::OddParity);
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
            QString str = getPortParams();
            emit portOpened(str);
        }
        else {
            emit portError(tr("Порт не доступен. Возможно он занят другим приложением."));
        }
    }
    else {
        qDebug() << "ComPort::onConnectClicked()"
                 << "serialPort.isOpen()";
    }
}

/**
 * @brief ComPort::onDisconnectClicked slot implementation.
 */
void ComPort::onDisconnectClicked()
{
    if (serialPort.isOpen()) {
        serialPort.close();
        emit portClosed();
    }
}

/**
 * @brief ComPort::getPortParams. Gets the parameters of the Com Port settings.
 * @return
 */
QString ComPort::getPortParams(void)
{
    QString str, str1;

    str = serialPort.portName();
    str1.setNum(serialPort.baudRate());
    str += "/" + str1 + "/";
    str1.setNum(serialPort.dataBits());
    str += str1 + "/";
    str1.setNum(serialPort.stopBits());
    str += str1 + "/";
    str1.setNum(serialPort.parity());
    str += str1 + "/";
    str1.setNum(serialPort.flowControl());
    str += str1;

    return str;
}

/**
 * @brief ComPort::onReadyRead slot implementation.
 */
void ComPort::onReadyRead()
{
    static QByteArray rxData;

    while(!serialPort.atEnd()) {
        rxData.append(serialPort.readAll());
        if (!serialPort.waitForReadyRead(30)) {
            break;
        }
    }

    if (rxData.size() > 0) {
        while(static_cast<quint8>(rxData[0]) == 0xFF) {
            rxData.remove(0, 1);    // Delete the preamble
        }
        emit parsingPacket(rxData); // Pass packet to the protocol
        rxData.clear();
    }
}

/**
 * @brief ComPort::onSendDataPacket slot implementation.
 * @param prea_size - size of the HART preamble
 * @param packet - data packet
 */
void ComPort::onSendDataPacket(const QByteArray &packet)
{   
    if (!serialPort.isOpen()) {
        emit portError(tr("Порт не открыт"));
    }
    else {
        QByteArray hartFrame;
        for (quint16 i = 0; i < PREAMBLE_SIZE; i++) {
            hartFrame.append(static_cast<char>(0xFF)); // Add the preamble
        }
        hartFrame.append(packet);        
        serialPort.write(hartFrame);
    }
}
//eof
