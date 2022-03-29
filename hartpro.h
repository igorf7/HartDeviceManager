#ifndef HARTPRO_H
#define HARTPRO_H

#include "hart.h"
#include "device.h"
#include <QList>
#include <QQueue>
#include <QObject>

#define LOADING_TIMEOUT     3000
#define RESPONSE_TIMEOUT    1500
#define POLLING_TIMEOUT     1500

class HartPro : public QObject
{
    Q_OBJECT
public:
    explicit HartPro(QObject *parent = nullptr);
     ~HartPro();

    QList<Device*> deviceList;
    QWidget* mainWidget;
    quint8 masterType;


private:
    QByteArray dataBuffer;
    quint8 pollingAddress;
    quint64 uniqueAddress;
    HartShortFrame_t* rxShortFrame;
    HartShortFrame_t* txShortFrame;
    HartLongFrame_t* rxLongFrame;
    HartLongFrame_t* txLongFrame;
    quint8 frameType;
    quint8 addressType;

    bool isPortBusy;
    bool isPollingRun;
    QQueue<QByteArray> packetQueue;

    void writeDataChunck(HartCommand_t cmd);
    void createHartFrame(HartCommand_t cmd, const QByteArray &cmd_data);
    quint8 calcChecksum(const char* data, int len);
    void linkErrorHandler(quint8 err_code);
    void cmdRespHandler(quint8 err_code, quint8 cmd_code);
    void devErrorHandler(quint8 err_code, quint8 cmd_code);
    bool checkPacketQueue();

protected:
    int respTaValue;
    int responseTimeout;
    void timerEvent(QTimerEvent *event);

signals:
    void eraseComplete();
    void transactionComplete(quint8 response, const QVariant &prm2);
    void loadingComplete();
    void sendDataPacket(const QByteArray &packet);
    void fillTableWidget(const QList<Device*> &list);
    void showPolligProgress(quint8 val);
    void searchCompleted();
    void writeStatusBar(const QString &str, int timeout);
    void showPacketSize(int size);

public slots:
    void onParsingPacket(const QByteArray &packet);
    void onSendCommand(HartCommand_t cmd, const QByteArray &cmd_data);
    void onSetDeviceAddress(quint64 addr);
    void onPortError();
    void onClearPacketQueue();
    void onSetMainWidget(QWidget* widget);

private slots:

};

#endif // HARTPRO_H
//eof
