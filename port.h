#ifndef PORT_H
#define PORT_H

#include "hart.h"
#include <QObject>
#include <QSerialPort>

class ComPort : public QObject
{
    Q_OBJECT
public:
    explicit ComPort(QObject *parent = nullptr);
     ~ComPort();
    QSerialPort serialPort;

signals:
    void portError(const QString &msg);
    void quitComPort();
    void portOpened(const QString &str);
    void portClosed();
    void parsingPacket(const QByteArray &packet);

public slots:
    void onPortStart();
    void onDisconnectClicked();
    void onConnectClicked(const QString &port);
    void onSendDataPacket(const QByteArray &packet);

private slots:    
    void onReadyRead();
    QString getPortParams();
};

#endif // PORT_H
//eof
