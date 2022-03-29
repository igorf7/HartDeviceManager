#include "configviewer.h"
#include "convert.h"

#include <qdebug.h>
#include <mem.h>
#include <QThread>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @brief ConfigViewer::ConfigViewer constructor
 * @param parent
 */
ConfigViewer::ConfigViewer(QObject *parent) : QObject(parent)
{
    currentDevice = nullptr;
    confGroupBox = new QGroupBox();
    confGroupBox->setTitle("Работа с устройствами в режиме конфигурации");
    confGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QStringList stringList;
    int i;

    /* GroupBox "Identificator" */
    QGroupBox* idGroupBox = new QGroupBox("Идентификатор устройства", confGroupBox);
    QLabel* shortAddrLabel = new QLabel("Короткий адрес");
    shortAddrSpinBox = new QSpinBox();
    QPushButton* shortAddrButton = new QPushButton("Записать");
    shortAddrButton->setAutoDefault(true);
    QLabel* fullAddrLabel = new QLabel("Полный адрес");
    fullAddrLineEdit = new QLineEdit();
    fullAddrLineEdit->setReadOnly(true);
    shortAddrSpinBox->setMinimum(0);
    shortAddrSpinBox->setMaximum(63);
    shortAddrSpinBox->setSingleStep(1);
    QHBoxLayout* idLayout = new QHBoxLayout();
    idLayout->addWidget(shortAddrLabel);
    idLayout->addWidget(shortAddrSpinBox);//, 1, Qt::AlignLeft);
    idLayout->addWidget(shortAddrButton);
    idLayout->addStretch();
    idLayout->addWidget(fullAddrLabel);
    idLayout->addWidget(fullAddrLineEdit, 0, Qt::AlignLeft);
    idGroupBox->setLayout(idLayout);

    /* GrpupBox "Information" */
    QGroupBox* infoGroupBox = new QGroupBox("Информация об устройстве", confGroupBox);
    QLabel* info1Label[info1ColumnSize];
    QLabel* info2Label[info1ColumnSize];
    dateEdit = new QDateEdit;
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());

    stringList<<"Тэг"<<"Дескриптор"<<"Сообщение"<<"Длинный тэг"<<"Производитель"<<"Дистрибьютор"
              <<"Серийный номер"<<"Тип устройства"<<"Название"<<"Версия HART"<<"Версия ПО"<<"Дата";

    QGridLayout* grid1Layout = new QGridLayout;
    for (i = 0; i < info1ColumnSize; i++) {
        info1Label[i] = new QLabel;
        info1Label[i]->setText(stringList.at(i));
        info2Label[i] = new QLabel;
        info2Label[i]->setText(stringList.at(i+info1ColumnSize));
        info1LineEdit[i] = new QLineEdit;
        grid1Layout->addWidget(info1Label[i], i, 0);
        grid1Layout->addWidget(info1LineEdit[i], i, 1);

    }
    QGridLayout* grid2Layout = new QGridLayout;
    for (i = 0; i < info2ColumnSize; i++) {
        info2LineEdit[i] = new QLineEdit;
        info2LineEdit[i]->setReadOnly(true);
        grid2Layout->addWidget(info2Label[i], i, 0);
        grid2Layout->addWidget(info2LineEdit[i], i, 1);
    }
    grid2Layout->addWidget(info2Label[i]);
    grid2Layout->addWidget(dateEdit);

    QRegExp regExp("^.{8}$");
    QValidator *validator = new QRegExpValidator(regExp, this);
    info1LineEdit[0]->setValidator(validator); // validator for tag
    regExp.setPattern("^.{16}$");
    validator = new QRegExpValidator(regExp, this);
    info1LineEdit[1]->setValidator(validator); // validator for descriptor
    regExp.setPattern("^.{32}$");
    validator = new QRegExpValidator(regExp, this);
    info1LineEdit[2]->setValidator(validator); // validator for message
    info1LineEdit[3]->setValidator(validator); // validator for long tag
    info1LineEdit[4]->setReadOnly(true);
    info1LineEdit[5]->setReadOnly(true);

    QHBoxLayout* infoLayout = new QHBoxLayout;
    infoLayout->addLayout(grid1Layout);
    infoLayout->addStretch();
    infoLayout->addLayout(grid2Layout);
    QPushButton* infoReadButton = new QPushButton("Чтение");
    infoReadButton->setAutoDefault(true);
    QPushButton* infoWriteButton = new QPushButton("Запись");
    infoWriteButton->setAutoDefault(true);
    QHBoxLayout* inforwLayout = new QHBoxLayout;
    inforwLayout->addWidget(infoReadButton, 0, Qt::AlignRight);
    inforwLayout->addWidget(infoWriteButton, 0, Qt::AlignLeft);
    QVBoxLayout* infoGroupLayout = new QVBoxLayout;
    infoGroupLayout->addLayout(infoLayout);
    infoGroupLayout->addLayout(inforwLayout);
    infoGroupBox->setLayout(infoGroupLayout);

    /* GroupBox "Device Parameters" */
    QGroupBox* prmGroupBox = new QGroupBox("Параметры устройства", confGroupBox);

    QLabel* prm1Label[prmColumnSize];
    QLabel* prm2Label[prmColumnSize];
    QDoubleValidator* doubleValidator =  new QDoubleValidator(0.0, 1000.0, 5);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    stringList.clear();
    stringList<<"Нижний предел 1-й пер."<<"Верхний предел 1-й пер."<<"Время демпфирования"
              <<"Передаточная функция"<<"Ед. измерения 1-й пер."<<"Ед. измерения 2-й пер."
              <<"Ед. измерения 3-й пер."<<"Ед. измерения 4-й пер.";

    QGridLayout* grid4Layout = new QGridLayout;
    for (i = 0; i < prmColumnSize; i++) {
        prm1Label[i] = new QLabel;
        prm1Label[i]->setText(stringList.at(i));
        prm2Label[i] = new QLabel;
        prm2Label[i]->setText(stringList.at(i+prmColumnSize));
        grid4Layout->addWidget(prm2Label[i], i, 0);
        unitComboBox[i] = new QComboBox;
        grid4Layout->addWidget(unitComboBox[i], i, 1);
    }
    QGridLayout* grid3Layout = new QGridLayout;
    for (i = 0; i < prmColumnSize-1; i++) {
        prm1LineEdit[i] = new QLineEdit;
        prm1LineEdit[i]->setValidator(doubleValidator);
        grid3Layout->addWidget(prm1Label[i], i, 0);
        grid3Layout->addWidget(prm1LineEdit[i], i, 1);
    }
    tranFuncBox = new QComboBox;
    grid3Layout->addWidget(prm1Label[i], i, 0);
    grid3Layout->addWidget(tranFuncBox, i, 1);
    prm1LineEdit[demp_time]->setReadOnly(true);

    QHBoxLayout* prmLayout = new QHBoxLayout;
    prmLayout->addLayout(grid3Layout);
    prmLayout->addStretch();
    prmLayout->addLayout(grid4Layout);
    QPushButton* prmReadButton = new QPushButton("Чтение");
    prmReadButton->setAutoDefault(true);
    QPushButton* prmWriteButton = new QPushButton("Запись");
    prmWriteButton->setAutoDefault(true);
    QHBoxLayout* prmrwLayout = new QHBoxLayout;
    prmrwLayout->addWidget(prmReadButton, 0, Qt::AlignRight);
    prmrwLayout->addWidget(prmWriteButton, 0, Qt::AlignLeft);
    QVBoxLayout* prmGroupLayout = new QVBoxLayout;
    prmGroupLayout->addLayout(prmLayout);
    prmGroupLayout->addLayout(prmrwLayout);
    prmGroupBox->setLayout(prmGroupLayout);

    /* Set the GroupBoxes */
    QVBoxLayout* configLayout = new QVBoxLayout;
    configLayout->addStretch();
    configLayout->addWidget(idGroupBox);
    configLayout->addWidget(infoGroupBox);
    configLayout->addWidget(prmGroupBox);
    confGroupBox->setLayout(configLayout);

    connect(shortAddrButton, SIGNAL(clicked()), this, SLOT(onShortAddrButton_clicked()));
    connect(infoReadButton, SIGNAL(clicked()), this, SLOT(onInfoReadButton_clicked()));
    connect(infoWriteButton, SIGNAL(clicked()), this, SLOT(onInfoWriteButton_clicked()));
    connect(prmReadButton, SIGNAL(clicked()), this, SLOT(onPrmReadButton_clicked()));
    connect(prmWriteButton, SIGNAL(clicked()), this, SLOT(onPrmWriteButton_clicked()));

    if (currentDevice != nullptr) {
        emit clearPacketQueue();
        onInfoReadButton_clicked();
        onPrmReadButton_clicked();
    }
}

/**
 * @brief ConfigViewer::~ConfigViewer destructor
 */
ConfigViewer::~ConfigViewer()
{
    delete confGroupBox;
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief ConfigViewer::onChangeTabIndex. Set information about current tab index
 * @param index - current tab index

void ConfigViewer::onChangeTabIndex(int index)
{
    currentTabIndex = index;

    if ((currentDevice != nullptr) && (currentTabIndex == myTabIndex)) {
        emit clearPacketQueue();
        onInfoReadButton_clicked();
        onPrmReadButton_clicked();
    }
} */

/**
 * @brief ConfigViewer::onShortAddrButton_clicked
 */
void ConfigViewer::onShortAddrButton_clicked()
{
    if (currentDevice == nullptr) return;

    QByteArray cmdData;
    cmdData.append(static_cast<char>(shortAddrSpinBox->value()));

    emit sendCommand(COMMAND_6, cmdData);
}

/**
 * @brief ConfigViewer::onInfoReadButton_clicked
 */
void ConfigViewer::onInfoReadButton_clicked()
{
    if (currentDevice == nullptr) return;

    info1LineEdit[tag]->setText("");
    info1LineEdit[descriptor]->setText("");
    info1LineEdit[message]->setText("");
    info1LineEdit[longTag]->setText("");

    dateEdit->setDate(QDate::currentDate());
    emit sendCommand(COMMAND_12, nullptr); // read messsage
    emit sendCommand(COMMAND_13, nullptr); // read tag, descriptor & date
    emit sendCommand(COMMAND_20, nullptr); // read long tag
}

/**
 * @brief ConfigViewer::onInfoWriteButton_clicked
 */
void ConfigViewer::onInfoWriteButton_clicked()
{
    if (currentDevice == nullptr) return;

    QByteArray str;
    QByteArray pack_str;

    /* Write message (Command 17) */
    str.append(info1LineEdit[message]->text().toUpper());
    while(str.size() < 32)
    {
        str.append(' ');
    }
    packString(str, pack_str);
    info1LineEdit[message]->setText("");

    emit sendCommand(COMMAND_17, pack_str);

    /* Write tag, descriptor and date (Command 18) */
    str.clear();
    pack_str.clear();
    str.append(info1LineEdit[tag]->text().toUpper());
    info1LineEdit[tag]->setText("");
    while(str.size() < 8)
    {
        str.append(' ');
    }
    packString(str, pack_str);
    str.clear();
    str.append(info1LineEdit[descriptor]->text().toUpper());
    info1LineEdit[descriptor]->setText("");
    while(str.size() < 16)
    {
        str.append(' ');
    }
    packString(str, pack_str);
    QDate date;
    date = dateEdit->date();    
    pack_str.append(static_cast<char>(date.day()));
    pack_str.append(static_cast<char>(date.month()));
    pack_str.append(static_cast<char>(date.year()-1900));
    date = QDate::currentDate();
    dateEdit->setDate(date);

    emit sendCommand(COMMAND_18, pack_str);

    /* Write long tag (Command 22) */
    str.clear();
    str.append(info1LineEdit[longTag]->text());
    info1LineEdit[longTag]->setText("");
    while(str.size() < 32)
    {
        str.append(' ');
    }

    emit sendCommand(COMMAND_22, str);
}

/**
 * @brief ConfigViewer::onPrmReadButton_clicked
 */
void ConfigViewer::onPrmReadButton_clicked()
{
    if (currentDevice == nullptr) return;

    QByteArray cmdData;

    prm1LineEdit[low_lim]->setText("");
    prm1LineEdit[high_lim]->setText("");
    prm1LineEdit[demp_time]->setText("");
    tranFuncBox->setCurrentIndex(0);
    for (int i = 0; i < unitComboNum; i++) {
        unitComboBox[i]->setEnabled(true);
        unitComboBox[i]->setCurrentIndex(0);
    }

    for (char i = 0; i < 4; i++) {
        cmdData.append(i);
    }
    emit sendCommand(COMMAND_9, cmdData); // read device variables with status
    emit sendCommand(COMMAND_15, nullptr); // read device information
}

/**
 * @brief ConfigViewer::onPrmWriteButton_clicked
 */
void ConfigViewer::onPrmWriteButton_clicked()
{
    if (currentDevice == nullptr) return;

    QByteArray cmdData;

    FloatToByteArray(prm1LineEdit[demp_time]->text().toFloat(), cmdData);
    emit sendCommand(COMMAND_34, cmdData); // Write Primary Variable Damping Value
    cmdData.clear();
    cmdData.append(currentDevice->getUnitCode(unitComboBox[0]->currentIndex()));
    FloatToByteArray(prm1LineEdit[high_lim]->text().toFloat(), cmdData);
    FloatToByteArray(prm1LineEdit[low_lim]->text().toFloat(), cmdData);
    emit sendCommand(COMMAND_35, cmdData); // Write Primary Variable Range Values
    cmdData.clear();
    cmdData.append(currentDevice->getUnitCode(unitComboBox[0]->currentIndex()));
    emit sendCommand(COMMAND_44, cmdData); // Write Primary Variable Units

    onPrmReadButton_clicked();
}

/**
 * @brief ConfigViewer::onTransactionComplete
 * @param response - response code
 * @param prm2 - extra parameter
 */
void ConfigViewer::onTransactionComplete(quint8 response, const QVariant &prm2)
{            
    QByteArray unpackedString;
    QByteArray packedString;
    QByteArray tmpBuffer;
    QDate tmpDate;

    switch(response)
    {
        case COMMAND_6:
            QMessageBox::information(confGroupBox, tr("Подтверждение"), "Короткий адрес записан.\n"
                                                 "Новый адрес: "+QString::number(prm2.toInt()));
            break;
        case COMMAND_9:
            tmpBuffer.append(prm2.toByteArray());
            for (int i = 0, k; i < 4; i++) {
                k = 3 + i * 8;
                if (tmpBuffer.size() >= (k+10)) {
                    unitComboBox[i]->setCurrentIndex(currentDevice->getUnitIndex(tmpBuffer.at(k)));
                    if (unitComboBox[i]->currentText() == "не исп.") unitComboBox[i]->setEnabled(false);
                }
            }
            break;
        case COMMAND_12:
            /* Message */
            packedString.append(prm2.toByteArray());
            unpackString(packedString, unpackedString);
            info1LineEdit[message]->setText(unpackedString);
            break;
        case COMMAND_13:            
            tmpBuffer.append(prm2.toByteArray());
            /* Tag */
            for (int i = 0; i < TAG_SIZE; i++) {
                packedString.append(tmpBuffer.at(i));
            }
            unpackString(packedString, unpackedString);
            info1LineEdit[tag]->setText(unpackedString);
            packedString.clear();
            unpackedString.clear();
            tmpBuffer.remove(0, TAG_SIZE);
            /* Descriptor */
            for (int i = 0; i < DESCRIPTOR_SIZE; i++) {
                packedString.append(tmpBuffer.at(i));
            }
            unpackString(packedString, unpackedString);
            info1LineEdit[descriptor]->setText(unpackedString);
            packedString.clear();
            unpackedString.clear();
            tmpBuffer.remove(0, DESCRIPTOR_SIZE);
            /* Date */
            tmpDate.setDate(tmpBuffer.at(2)+1900, tmpBuffer.at(1), tmpBuffer.at(0));
            dateEdit->setDate(tmpDate);
            break;
        case COMMAND_15:
            tmpBuffer.append(prm2.toByteArray());            
            prm1LineEdit[high_lim]->setText(QString::number(static_cast<double>(ByteArrayToFloat(tmpBuffer, 3))));
            prm1LineEdit[low_lim]->setText(QString::number(static_cast<double>(ByteArrayToFloat(tmpBuffer, 7))));
            prm1LineEdit[demp_time]->setText(QString::number(static_cast<double>(ByteArrayToFloat(tmpBuffer, 11))));
            tranFuncBox->setCurrentIndex(tmpBuffer.at(1)+1);
            break;
        case COMMAND_20:
            /* Long Tag */
            unpackedString.append(prm2.toByteArray());
            info1LineEdit[longTag]->setText(unpackedString);
            break;
        default:
            break;
    }
}

/**
 * @brief ConfigViewer::onSelectDevice
 * @param list - pointer to the device list
 * @param index - device index in the list
 */
void ConfigViewer::onSelectDevice(Device *device)
{
    currentDevice = device; // set current device pointer
    emit setDeviceAddress(currentDevice->getFullAddress());
    tranFuncBox->clear();
    tranFuncBox->addItems(currentDevice->funcList);

    for (int i = 0; i < unitComboNum; i++) {
        unitComboBox[i]->clear();
        unitComboBox[i]->setEnabled(true);
        unitComboBox[i]->addItems(currentDevice->unitsList);
    }

    quint16 utmp;

    shortAddrSpinBox->setValue(currentDevice->getShortAddress());
    info2LineEdit[serial]->setText(QString::number(currentDevice->getSerial())+
                            "   (0x"+QString::number(currentDevice->getSerial(), 16).toUpper()+")");

    fullAddrLineEdit->setText("0x"+QString::number(currentDevice->getFullAddress(), 16).toUpper());
    utmp = currentDevice->getTypeCode();
    info2LineEdit[devType]->setText(QString::number(utmp)+
                             "   (0x"+QString::number(utmp, 16).toUpper()+")");

    info2LineEdit[devName]->setText(currentDevice->getTypeStr(utmp));
    info2LineEdit[hartVer]->setText("HART "+QString::number(currentDevice->getUnivCmdRev()));
    info2LineEdit[swVer]->setText(QString::number(currentDevice->getSoftRevision()));

    utmp = currentDevice->getVendorCode();
    info1LineEdit[vendor]->setText(currentDevice->getVendorStr(utmp)+
                            "   (0x"+QString::number(utmp, 16).toUpper()+")");

    utmp = currentDevice->getDistribCode();
    info1LineEdit[distributer]->setText(currentDevice->getDistribStr(utmp)+
                           "   (0x"+QString::number(utmp, 16).toUpper()+")");

    onInfoReadButton_clicked();
    onPrmReadButton_clicked();
}

/**
 * @brief ConfigViewer::onResetDeviceData
 */
void ConfigViewer::onResetDeviceData()
{
    for (int i = 0; i < info1ColumnSize; i++) {
        info1LineEdit[i]->setText("");
    }
    for (int i = 0; i < info2ColumnSize; i++) {
        info2LineEdit[i]->setText("");
    }
    for (int i = 0; i < prmColumnSize-1; i++) {
        prm1LineEdit[i]->setText("");
    }
    for (int i = 0; i < unitComboNum; i++) {
        unitComboBox[i]->setCurrentIndex(0);
    }
    tranFuncBox->setCurrentIndex(0);
}

/**
 * @brief ConfigViewer::packString
 * @param str
 * @param packed_str
 */
void ConfigViewer::packString(const QByteArray &str, QByteArray &packed_str)
{
    qint32 shiftBuffer;
    int index = 0;

    int strSize = str.size();

    /* Pack four, 6 bit-ASCII characters into three bytes */
    for (int i = 0; i < strSize/4; i++) {
        shiftBuffer = 0;
        shiftBuffer |= (str.at(index++) << 2) & 0xFF;
        shiftBuffer <<= 6;
        shiftBuffer |= (str.at(index++) << 2) & 0xFF;
        shiftBuffer <<= 6;
        shiftBuffer |= (str.at(index++) << 2) & 0xFF;
        shiftBuffer <<= 6;
        shiftBuffer |= (str.at(index++) << 2) & 0xFF;
        shiftBuffer >>= 2;

        /* Add processed data to a packed string */
        packed_str.append(static_cast<char>(shiftBuffer>>16));
        packed_str.append(static_cast<char>(shiftBuffer>>8));
        packed_str.append(static_cast<char>(shiftBuffer>>0));
    }
}

/**
 * @brief ConfigViewer::unpackString
 * @param str
 * @param unpacked_str
 */
void ConfigViewer::unpackString(const QByteArray &str, QByteArray &unpacked_str)
{
    qint32 shiftBuffer;
    int index = 0;

    int strSize = str.size();

    /* Unpack the four, 6-bit ASCII characters */
    for (int i = 0; i < strSize/3; i++) {
        shiftBuffer = 0;
        shiftBuffer |= (str.at(index) >> 2) & 0x3F;
        shiftBuffer <<= 8;
        shiftBuffer |= (str.at(index++) & 0x03) << 4;
        shiftBuffer |= (str.at(index) >> 4) & 0x0F;
        shiftBuffer <<= 8;
        shiftBuffer |= (str.at(index++) & 0x0F) << 2;
        shiftBuffer |= (str.at(index) >> 6) & 0x03;
        shiftBuffer <<= 8;
        shiftBuffer |= str.at(index++) & 0x3F;

        /* Place the complement of Bit 5 into Bit 6 */
        if (shiftBuffer & 0x20)
            shiftBuffer &= ~0x40;
        else
            shiftBuffer |= 0x40;

        if (shiftBuffer & 0x2000)
            shiftBuffer &= ~0x4000;
        else
            shiftBuffer |= 0x4000;

        if (shiftBuffer & 0x200000)
            shiftBuffer &= ~0x400000;
        else
            shiftBuffer |= 0x400000;

        if (shiftBuffer & 0x20000000)
            shiftBuffer &= ~0x40000000;
        else
            shiftBuffer |= 0x40000000;

        /* Add processed data to the unpacked string */
        unpacked_str.append(static_cast<char>(shiftBuffer>>24));
        unpacked_str.append(static_cast<char>(shiftBuffer>>16));
        unpacked_str.append(static_cast<char>(shiftBuffer>>8));
        unpacked_str.append(static_cast<char>(shiftBuffer>>0));
    }
}
//eof
