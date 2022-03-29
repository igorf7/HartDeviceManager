#include "device.h"

#include <QDebug>
/**
 * @brief Device::Device constructor
 * @param parent
 */
Device::Device(QObject *parent) : QObject(parent)
{
    /* Set values by default */
    shortAddress = 0;
    fullAddress = 0;
    devSerial = 0;
    deviceType = v318i;
    vendorCode = Alcont;
    distribCode = Alcont;
    devProfile = AutomationDevice;
    confChangeCnt = 0;
    extention = 254;
    reqPreaNum = 5;
    univCmdRev = 7;
    specCmdRev = 7;
    softRevision = 1;
    hardRevision = 0;
    flags = 0;
    respPreaNum = 5;
    deviceVarNum = 0;
    extendedStaus = 0;

    funcList<<""<<"Линейная"<<"Корнеизвлекающая"<<"Корнеизвл.3-й степ"<<
                    "Корнеизвл.3-й степ"<<"Спец. кривая"<<"Квадратичная";

    unitsList<<""<<"m/s"<<"m/s²"<<"°C"<<"°F"<<"°K"<<"Па"<<"кПа"<<"МПа"<<"кгс/см²"<<"Не исп.";

    unitsTable.insert(21, 1);
    unitsTable.insert(172, 2);
    unitsTable.insert(32, 3);
    unitsTable.insert(33, 4);
    unitsTable.insert(35, 5);
    unitsTable.insert(11, 6);
    unitsTable.insert(12, 7);
    unitsTable.insert(237, 8);
    unitsTable.insert(10, 9);
    unitsTable.insert(250, 10);

    unitsStrTable.insert(21, "m/s");
    unitsStrTable.insert(172, "m/s²");
    unitsStrTable.insert(32, "°C");
    unitsStrTable.insert(33, "°F");
    unitsStrTable.insert(35, "°K");
    unitsStrTable.insert(11, "Па");
    unitsStrTable.insert(12, "кПа");
    unitsStrTable.insert(237, "МПа");
    unitsStrTable.insert(10, "кгс/см²");
    unitsStrTable.insert(250, "Не исп.");

    typeStrTable.insert(0xE42D, "V-318-i");
    vendorStrTable.insert(0x6116, "ALCONT Ltd");
    distrStrTable.insert(0x6116, "ALCONT Ltd");
}

/**
 * @brief Device::~Device destructor
 */
Device::~Device()
{
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief Device::setFullAddress
 */
void Device::setFullAddress()
{
    this->fullAddress = (this->deviceType & 0x3FFF);
    this->fullAddress <<= 24;
    this->fullAddress |= this->devSerial;
}

/**
 * @brief Device::getFullAddress
 * @return
 */
quint64 Device::getFullAddress()
{
    return this->fullAddress;
}

/**
 * @brief Device::setShortAddress
 * @param addr
 */
void Device::setShortAddress(quint8 addr)
{
    this->shortAddress = addr;
}

/**
 * @brief Device::getShortAddress
 * @return
 */
quint8 Device::getShortAddress()
{
    return this->shortAddress;
}

/**
 * @brief Device::setSerial
 * @param serial
 */
void Device::setSerial(quint32 serial)
{
    this->devSerial = serial;
}

/**
 * @brief Device::getSerial
 * @return
 */
quint32 Device::getSerial()
{
    return this->devSerial;
}

/**
 * @brief Device::setType
 * @param type
 */
void Device::setType(DevType_t type)
{
    this->deviceType = type;
}

/**
 * @brief Device::getTypeCode
 * @return type code of the device
 */
DevType_t Device::getTypeCode()
{
    return this->deviceType;
}

/**
 * @brief Device::getTypeStr
 * @param key
 * @return String name of the device
 */
QString Device::getTypeStr(quint16 key)
{
    return typeStrTable.value(key);
}

/**
 * @brief Device::setVendorCode
 * @param code
 */
void Device::setVendorCode(Vendor_t code)
{
    this->vendorCode = code;
}

/**
 * @brief Device::getVendorCode
 * @return
 */
Vendor_t Device::getVendorCode()
{
    return this->vendorCode;
}

/**
 * @brief Device::getVendorStr
 * @param key
 * @return String name of the vendor
 */
QString Device::getVendorStr(quint16 key)
{
    return vendorStrTable.value(key);
}

/**
 * @brief Device::setDistribCode
 * @param code
 */
void Device::setDistribCode(Vendor_t code)
{
    this->distribCode = code;
}

/**
 * @brief Device::getDistribCode
 * @return
 */
Vendor_t Device::getDistribCode()
{
    return this->distribCode;
}

/**
 * @brief Device::getDistribStr
 * @param key
 * @return String name of the distributer
 */
QString Device::getDistribStr(quint16 key)
{
    return distrStrTable.value(key);
}

/**
 * @brief Device::setProfile
 * @param profile
 */
void Device::setProfile(Profile_t profile)
{
    this->devProfile = profile;
}

/**
 * @brief Device::getProfile
 * @return
 */
Profile_t Device::getProfile()
{
    return this->devProfile;
}

/**
 * @brief Device::setConfChangeCnt
 * @param cnt
 */
void Device::setConfChangeCnt(quint16 cnt)
{
    this->confChangeCnt = cnt;
}

/**
 * @brief Device::getConfChangeCnt
 * @return
 */
quint16 Device::getConfChangeCnt()
{
    return this->confChangeCnt;
}

/**
 * @brief Device::setExtention
 * @param extention
 */
void Device::setExtention(quint8 extention)
{
    this->extention = extention;
}

/**
 * @brief Device::getExtention
 * @return
 */
quint8 Device::getExtention()
{
    return this->extention;
}

/**
 * @brief Device::setReqPreaNum
 * @param num
 */
void Device::setReqPreaNum(quint8 num)
{
    this->reqPreaNum = num;
}

/**
 * @brief Device::getReqPreaNum
 * @return
 */
quint8 Device::getReqPreaNum()
{
    return this->reqPreaNum;
}

/**
 * @brief Device::setUnivCmdRev
 * @param rev
 */
void Device::setUnivCmdRev(quint8 rev)
{
    this->univCmdRev = rev;
}

/**
 * @brief Device::getUnivCmdRev
 * @return
 */
quint8 Device::getUnivCmdRev()
{
    return this->univCmdRev;
}

/**
 * @brief Device::setSpecCmdRev
 * @param rev
 */
void Device::setSpecCmdRev(quint8 rev)
{
    this->specCmdRev = rev;
}

/**
 * @brief Device::getSpecCmdRev
 * @return
 */
quint8 Device::getSpecCmdRev()
{
    return this->specCmdRev;
}

/**
 * @brief Device::setSoftRevision
 * @param rev
 */
void Device::setSoftRevision(quint8 rev)
{
    this->softRevision = rev;
}

/**
 * @brief Device::getSoftRevision
 * @return
 */
quint8 Device::getSoftRevision()
{
    return this->softRevision;
}

/**
 * @brief setHardRevision
 * @param rev
 */
void Device::setHardRevision(quint8 rev)
{
    this->hardRevision = rev;
}

/**
 * @brief Device::getHardRevision
 * @return
 */
quint8 Device::getHardRevision()
{
    return this->hardRevision;
}

/**
 * @brief Device::setFlags
 * @param flags
 */
void Device::setFlags(quint8 flags)
{
    this->flags = flags;
}

/**
 * @brief Device::getFlags
 * @return
 */
quint8 Device::getFlags()
{
    return this->flags;
}

/**
 * @brief Device::setRespPreaNum
 * @param num
 */
void Device::setRespPreaNum(quint8 num)
{
    this->respPreaNum = num;
}

/**
 * @brief Device::getRespPreaNum
 * @return
 */
quint8 Device::getRespPreaNum()
{
    return this->respPreaNum;
}

/**
 * @brief Device::setDeviceVarNum
 * @param num
 */
void Device::setDeviceVarNum(quint8 num)
{
    this->deviceVarNum = num;
}

/**
 * @brief Device::getDeviceVarNum
 * @return
 */
quint8 Device::getDeviceVarNum()
{
    return this->deviceVarNum;
}

/**
 * @brief Device::setExtendedStatus
 * @param status
 */
void Device::setExtendedStatus(quint8 status)
{
    this->extendedStaus = status;
}

/**
 * @brief Device::getExtendedStatus
 * @return
 */
quint8 Device::getExtendedStatus()
{
    return this->extendedStaus;
}

/**
 * @brief Device::getUnitIndex
 * @param unitCode
 * @return
 */
int Device::getUnitIndex(char unitCode)
{
    return unitsTable.value(static_cast<quint8>(unitCode));
}

/**
 * @brief Device::getUnitIndex
 * @param unitIndex
 * @return
 */
char Device::getUnitCode(int unitIndex)
{
    return static_cast<char>(unitsTable.key(unitIndex));
}

/**
 * @brief Device::getUnitStr
 * @param key
 * @return
 */
QString Device::getUnitStr(quint8 key)
{
    return unitsStrTable.value(key);
}
//eof
