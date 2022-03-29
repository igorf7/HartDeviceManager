#ifndef CONFIGVIEWER_H
#define CONFIGVIEWER_H

#include "hart.h"
#include "device.h"

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDateEdit>

class ConfigViewer : public QObject
{
    Q_OBJECT
public:
    explicit ConfigViewer(QObject *parent = nullptr);
    ~ConfigViewer();
    QGroupBox* confGroupBox;

private:
    Device* currentDevice;
    QSpinBox* shortAddrSpinBox;
    QLineEdit* fullAddrLineEdit;
    QDateEdit* dateEdit;

    static const quint8 prmColumnSize = 4;
    static const quint8 info1ColumnSize = 6;
    static const quint8 info2ColumnSize = 5;
    QLineEdit* info1LineEdit[info1ColumnSize];
    QLineEdit* info2LineEdit[info2ColumnSize];
    QLineEdit* prm1LineEdit[prmColumnSize];
    static const quint8 unitComboNum = 4;
    QComboBox* tranFuncBox;
    QComboBox* unitComboBox[unitComboNum];

    enum{
        tag = 0,
        descriptor,
        message,
        longTag,
        vendor,
        distributer,
        serial = 0,
        devType,
        devName,
        hartVer,
        swVer,
        low_lim = 0,
        high_lim,
        demp_time
    }widgetIndex;

    void packString(const QByteArray &str, QByteArray &packed_str);
    void unpackString(const QByteArray &str, QByteArray &unpacked_str);

protected:

signals:
    void sendCommand(HartCommand_t cmd, const QByteArray &cmd_data);
    void setDeviceAddress(quint64 addr);
    void clearPacketQueue();

public slots:
    void onTransactionComplete(quint8 response, const QVariant &prm2);
    void onSelectDevice(Device *device);
    void onResetDeviceData();

private slots:
    void onShortAddrButton_clicked();
    void onInfoReadButton_clicked();
    void onInfoWriteButton_clicked();
    void onPrmReadButton_clicked();
    void onPrmWriteButton_clicked();
};

#endif // CONFIGVIEWER_H
