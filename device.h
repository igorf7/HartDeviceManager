#ifndef DEVICE_H
#define DEVICE_H

#include "hart.h"

#include <QObject>
#include <QHash>

class Device : public QObject
{
    Q_OBJECT
public:
    QStringList funcList;
    QStringList unitsList;

    explicit Device(QObject *parent = nullptr);
    ~Device();

    void setFullAddress();
    quint64 getFullAddress();
    void setShortAddress(quint8 addr);
    quint8 getShortAddress();
    void setSerial(quint32 serial);
    quint32 getSerial();
    void setType(DevType_t type);
    DevType_t getTypeCode();
    QString getTypeStr(quint16 key);
    void setVendorCode(Vendor_t code);
    Vendor_t getVendorCode();
    QString getVendorStr(quint16 key);
    void setDistribCode(Vendor_t code);
    Vendor_t getDistribCode();
    QString getDistribStr(quint16 key);
    void setProfile(Profile_t profile);
    Profile_t getProfile();
    void setConfChangeCnt(quint16 cnt);
    quint16 getConfChangeCnt();
    void setExtention(quint8 extention);
    quint8 getExtention();
    void setReqPreaNum(quint8 num);
    quint8 getReqPreaNum();
    void setUnivCmdRev(quint8 rev);
    quint8 getUnivCmdRev();
    void setSpecCmdRev(quint8 rev);
    quint8 getSpecCmdRev();
    void setSoftRevision(quint8 rev);
    quint8 getSoftRevision();
    void setHardRevision(quint8 rev);
    quint8 getHardRevision();
    void setFlags(quint8 flags);
    quint8 getFlags();
    void setRespPreaNum(quint8 num);
    quint8 getRespPreaNum();
    void setDeviceVarNum(quint8 num);
    quint8 getDeviceVarNum();
    void setExtendedStatus(quint8 status);
    quint8 getExtendedStatus();
    int getUnitIndex(char unitCode);
    char getUnitCode(int unitIndex);
    QString getUnitStr(quint8 key);

private:
    quint64 fullAddress;

    quint32 devSerial;
    DevType_t deviceType;

    Vendor_t vendorCode,
             distribCode;

    Profile_t devProfile;

    quint16 confChangeCnt;

    quint8 shortAddress,
           extention,
           reqPreaNum,
           univCmdRev,
           specCmdRev,
           softRevision,
           hardRevision,
           flags,
           respPreaNum,
           deviceVarNum,
           extendedStaus;

    QHash<quint8, int> unitsTable;          // unit code/unit idex (in spin box) table
    QHash<quint8, QString> unitsStrTable;   // unit code/unit string name table
    QHash<quint16, QString> typeStrTable;   // device type code/device string name table
    QHash<quint16, QString> vendorStrTable; // device manufacturrer code/string name table
    QHash<quint16, QString> distrStrTable;  // device distributer code/string name table
};
#endif // DEVICE_H
