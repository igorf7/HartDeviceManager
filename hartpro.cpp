#include "hartpro.h"

#include <qdebug.h>
#include <QThread>
#include <QMessageBox>
#include <QTimerEvent>

/**
 * @brief HartPro::HartPro constructor
 * @param parent
 */
HartPro::HartPro(QObject *parent) : QObject(parent)
{
    frameType = STX;    
    pollingAddress = 0;
    masterType = PRIMARY_MASTER;
    uniqueAddress = 0;
    isPortBusy = false;
    isPollingRun = false;
    respTaValue = RESPONSE_TIMEOUT;

    responseTimeout = 0;
}

/**
 * @brief HartPro::~HartPro destructor
 */
HartPro::~HartPro()
{        
    qDebug()<<"~By-by from"<<this;
}

/**
 * @brief HartPro::onSetMainWidget
 * @param widget
 */
void HartPro::onSetMainWidget(QWidget* widget)
{
    mainWidget = widget;
}

/**
 * @brief HartPro::calcChecksum. Compute the checksum for the data packet.
 * @param data - data packet
 * @param len - size of the data packet
 * @return computed checksum
 */
quint8 HartPro:: calcChecksum(const char* data, int len)
{
    quint8 checksum = static_cast<quint8>(data[0]);

    for (int i = 1; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

/**
 * @brief HartPro::onPortError
 */
void HartPro::onPortError()
{
    this->onSendCommand(STOP_CMD, nullptr);
}

/**
 * @brief HartPro::createHartFrame. Creates HART data frame.
 * @param cmd - command code
 * @param cmd_data - data, wich the command may contain
 */
void HartPro::createHartFrame(HartCommand_t cmd, const QByteArray &cmd_data)
{
    QByteArray packet;

    if (cmd == COMMAND_0) {
        packet.append(static_cast<char>(frameType|POLING_ADDR_TYPE));
        packet.append(static_cast<char>(pollingAddress|masterType));
    }
    else {
        packet.append(static_cast<char>(frameType|UNIQUE_ADDR_TYPE));
        packet.append(static_cast<char>((uniqueAddress>>32)|masterType));
        packet.append(static_cast<char>(uniqueAddress>>24));
        packet.append(static_cast<char>(uniqueAddress>>16));
        packet.append(static_cast<char>(uniqueAddress>>8));
        packet.append(static_cast<char>(uniqueAddress>>0));
    }
    packet.append(static_cast<char>(cmd));
    packet.append(static_cast<char>(cmd_data.size()));
    packet.append(cmd_data);
    packet.append(static_cast<char>(calcChecksum(packet.data(), packet.size())));

    if (isPortBusy) {
        packetQueue.enqueue(packet);
    }
    else {
        isPortBusy = true;
        if (cmd != COMMAND_0) {
            isPollingRun = false;
        }        
        responseTimeout = this->startTimer(respTaValue);
        emit sendDataPacket(packet);
    }
}

/**
 * @brief HartPro::writeDataChunck. Collects the data chunk for send.
 * @param cmd - command code
 */
void HartPro::writeDataChunck(HartCommand_t cmd)
{
    QByteArray dataChunck;

    if (dataBuffer.isEmpty()) {
        emit loadingComplete();
        qDebug() << "bufferSize = 0";
    }
    else {
        int bufferSize = dataBuffer.size();

        qDebug() << "bufferSize = " << bufferSize;

        if (bufferSize <= DATA_CHUNCK_SIZE) {
            dataChunck.append(dataBuffer);
            dataBuffer.clear();
        }
        else {
            dataChunck = dataBuffer.left(DATA_CHUNCK_SIZE);
            dataBuffer.remove(0, DATA_CHUNCK_SIZE);
        }
        createHartFrame(cmd, dataChunck);
    }
}

/**
 * @brief HartPro::onSendCommand slot implementation
 * @param cmd - command code
 * @param cmd_data - data, wich the command may contain
 */
void HartPro::onSendCommand(HartCommand_t cmd, const QByteArray &cmd_data)
{
    respTaValue = RESPONSE_TIMEOUT;

    /* Command processing */
    switch(cmd)
    {
        case COMMAND_0:
            respTaValue = POLLING_TIMEOUT;
            if (pollingAddress == 0) {
                deviceList.clear();
                isPollingRun = true;
            }
            createHartFrame(COMMAND_0, cmd_data);
            pollingAddress++;
            if (pollingAddress == 64) {
                pollingAddress = 0;
            }
            break;
        case COMMAND_1:
            createHartFrame(COMMAND_1, cmd_data);
            break;
         case COMMAND_2:
            createHartFrame(COMMAND_2, cmd_data);
            break;
        case COMMAND_6:
            createHartFrame(COMMAND_6, cmd_data);
            break;
        case COMMAND_9:
            createHartFrame(COMMAND_9, cmd_data);        
            break;
        case COMMAND_12:
            createHartFrame(COMMAND_12, cmd_data);
            break;
        case COMMAND_13:
            createHartFrame(COMMAND_13, cmd_data);
            break;
        case COMMAND_15:
            createHartFrame(COMMAND_15, cmd_data);
        break;
        case COMMAND_17:
            createHartFrame(COMMAND_17, cmd_data);
            break;
        case COMMAND_18:
            createHartFrame(COMMAND_18, cmd_data);
            break;
        case COMMAND_20:
            createHartFrame(COMMAND_20, cmd_data);
            break;
        case COMMAND_22:
            createHartFrame(COMMAND_22, cmd_data);
            break;
        case COMMAND_34:
            createHartFrame(COMMAND_34, cmd_data);
            break;
        case COMMAND_35:
            createHartFrame(COMMAND_35, cmd_data);
            break;
        case COMMAND_41:
            createHartFrame(COMMAND_41, cmd_data);
            break;
        case COMMAND_42:
            createHartFrame(COMMAND_42, cmd_data);
            break;
        case COMMAND_44:
            createHartFrame(COMMAND_44, cmd_data);
            break;
        case ERASE_APP_FLASH:
            createHartFrame(ERASE_APP_FLASH, cmd_data);
            break;
        case WRITE_FLASH_FIRST:
            dataBuffer.append(cmd_data);
            respTaValue = LOADING_TIMEOUT;
            writeDataChunck(WRITE_FLASH_FIRST);
            break;
        case WRITE_FLASH_NEXT:
            respTaValue = LOADING_TIMEOUT;
            writeDataChunck(WRITE_FLASH_NEXT);
            break;
        case CHECK_HASH_SUM:
            respTaValue = LOADING_TIMEOUT;
            createHartFrame(CHECK_HASH_SUM, cmd_data);
            break;
        case CHECK_MEMORY:
            respTaValue = LOADING_TIMEOUT;
            createHartFrame(CHECK_MEMORY, cmd_data);
            break;
        case START_APPLICATION:
            createHartFrame(START_APPLICATION, cmd_data);
            break;
        case SET_UNIQUE_ID:
            uniqueAddress = 0;
            respTaValue = LOADING_TIMEOUT;
            createHartFrame(SET_UNIQUE_ID, cmd_data);
            break;
        case CLEAR_CONFIG:
            createHartFrame(CLEAR_CONFIG, cmd_data);
            break;
        case STOP_CMD:  /* Abort all operations */
            pollingAddress = 0;
            isPortBusy = false;
            isPollingRun = false;
            if (responseTimeout != 0) {
                killTimer(responseTimeout);
                responseTimeout = 0;
            }
            if (!dataBuffer.isEmpty()) {
                dataBuffer.clear();
            }
            this->onClearPacketQueue();
            qDebug()<<"STOP_CMD";
            break;
        default:
            break;
    }
}

/**
 * @brief HartPro::onParsingPacket slot implementation
 * @param packet - received data packet
 */
void HartPro::onParsingPacket(const QByteArray &packet)
{
    quint8 rcv_check = static_cast<quint8>(packet.at(packet.size()-1));
    quint8 check_summ = calcChecksum(packet.data(), packet.size()-1);

    qint32 tmp = 0;
    QByteArray tmpArray;

    isPortBusy = false;

    if (responseTimeout != 0) {
        killTimer(responseTimeout);
        responseTimeout = 0;
    }

    if (rcv_check != check_summ) {
        //QMessageBox::warning(mainWidget, tr("Ошибка"), "Ошибка контрольной суммы пакета!");
        qDebug("Ошибка контрольной суммы пакета!");
    }
    else {
        emit writeStatusBar("Идёт обмен...", 1000);
        switch(packet.at(0) & UNIQUE_ADDR_TYPE)
        {
            case POLING_ADDR_TYPE: // Short frame
                rxShortFrame = reinterpret_cast<HartShortFrame_t*>(const_cast<char*>(packet.data()));
                qDebug("Short frame received.");
                if (rxShortFrame->command == COMMAND_0) {
                    Device* newDevice = new Device(this);
                    newDevice->setShortAddress(rxShortFrame->address & 0x3F);
                    newDevice->setExtention(rxShortFrame->data[2]);
                    newDevice->setType(static_cast<DevType_t>((rxShortFrame->data[3]<<8)|rxShortFrame->data[4]));
                    newDevice->setReqPreaNum(rxShortFrame->data[5]);
                    newDevice->setUnivCmdRev(rxShortFrame->data[6]);
                    newDevice->setSpecCmdRev(rxShortFrame->data[7]);
                    newDevice->setSoftRevision(rxShortFrame->data[8]);
                    newDevice->setHardRevision(rxShortFrame->data[9]>>3);
                    newDevice->setFlags(rxShortFrame->data[10]);
                    newDevice->setSerial(static_cast<quint32>((rxShortFrame->data[11]<<16)|(rxShortFrame->data[12]<<8)|rxShortFrame->data[13]));
                    newDevice->setRespPreaNum(rxShortFrame->data[14]);
                    newDevice->setDeviceVarNum(rxShortFrame->data[15]);
                    newDevice->setConfChangeCnt(static_cast<quint16>((rxShortFrame->data[16]<<8)|rxShortFrame->data[17]));
                    newDevice->setExtendedStatus(rxShortFrame->data[18]);
                    newDevice->setVendorCode(static_cast<Vendor_t>((rxShortFrame->data[19]<<8)|rxShortFrame->data[20]));
                    newDevice->setDistribCode(static_cast<Vendor_t>((rxShortFrame->data[21]<<8)|rxShortFrame->data[22]));
                    newDevice->setProfile(static_cast<Profile_t>(rxShortFrame->data[23]));
                    newDevice->setFullAddress();
                    deviceList.append(newDevice);                    
                    emit fillTableWidget(deviceList);
                }
                break;
            case UNIQUE_ADDR_TYPE: // Long frame
                rxLongFrame = reinterpret_cast<HartLongFrame_t*>(const_cast<char*>(packet.data()));
                if ((rxLongFrame->data[0]) != 0) { // check ststus byte_0
                    if (rxLongFrame->data[0] & 0x80) {
                        this->linkErrorHandler(rxLongFrame->data[0]);
                    }
                    else {
                        this->cmdRespHandler(rxLongFrame->data[0], rxLongFrame->command);
                    }
                }
                switch(rxLongFrame->command)
                {
                    case COMMAND_1:
                        for (tmp = 0; tmp < rxLongFrame->data_len-2; tmp++) {
                            tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                        }
                        emit transactionComplete(rxLongFrame->command, tmpArray);
                        break;
                    case COMMAND_2:
                        for (tmp = 0; tmp < rxLongFrame->data_len-2; tmp++) {
                            tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                        }
                        emit transactionComplete(rxLongFrame->command, tmpArray);
                        break;
                    case COMMAND_6:
                        emit transactionComplete(rxLongFrame->command, rxLongFrame->data[2]);
                        break;
                    case COMMAND_9:
                        for (tmp = 0; tmp < rxLongFrame->data_len-2; tmp++) {
                            tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                        }
                        emit transactionComplete(rxLongFrame->command, tmpArray);
                        break;
                    case COMMAND_12:
cmd_12:                if (rxLongFrame->data_len == MESSAGE_SIZE+2) {
                           for (tmp = 0; tmp < MESSAGE_SIZE; tmp++) {
                               tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                           }
                           emit transactionComplete(COMMAND_12, tmpArray);
                        }
                        else {
                            QMessageBox::critical(mainWidget, tr("Ошибка"), "Некорректный размер данных в ответе.\n"
                                                              "Код комады: "+QString::number(rxLongFrame->command));
                        }
                        break;
                    case COMMAND_13:
cmd_13:                 if (rxLongFrame->data_len == (TAG_SIZE+DESCRIPTOR_SIZE+DATE_SIZE+2)) {
                            for (tmp = 0; tmp < (TAG_SIZE+DESCRIPTOR_SIZE+DATE_SIZE); tmp++) {
                                tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                            }
                            emit transactionComplete(COMMAND_13, tmpArray);
                        }
                        else {
                            QMessageBox::critical(mainWidget, tr("Ошибка"), "Некорректный размер данных в ответе.\n"
                                                          "Код комады: "+QString::number(rxLongFrame->command));
                        }
                        break;
                    case COMMAND_15:
                        for (tmp = 0; tmp < rxLongFrame->data_len-2; tmp++) {
                            tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                        }
                        emit transactionComplete(rxLongFrame->command, tmpArray);
                        break;
                    case COMMAND_17:
                        emit writeStatusBar("Сообщение записано", 3000);
                        goto cmd_12;
                    case COMMAND_18:
                        emit writeStatusBar("Тэг, дескриптор и дата записаны", 3000);
                        goto cmd_13;
                    case COMMAND_20:
cmd_20:                 if (rxLongFrame->data_len == LONGTAG_SIZE+2) {
                            for (tmp = 0; tmp < LONGTAG_SIZE; tmp++) {
                                tmpArray.append(static_cast<char>(rxLongFrame->data[2+tmp]));
                            }
                            emit transactionComplete(COMMAND_20, tmpArray);
                        }
                        else {
                            QMessageBox::critical(mainWidget, tr("Ошибка"), "Некорректный размер данных в ответе.\n"
                                                          "Код комады: "+QString::number(rxLongFrame->command));
                        }
                        break;
                    case COMMAND_22:
                        emit writeStatusBar("Длинный тэг записан", 3000);
                        goto cmd_20;
                    case COMMAND_34:
                        emit writeStatusBar("Время демпфирования записано", 3000);
                        break;
                    case COMMAND_35:
                        emit writeStatusBar("Нижний и верхний пределы записаны", 3000);
                        break;
                    case COMMAND_42:
                        QMessageBox::information(mainWidget, tr("Подтверждение"), "Выполнен рестарт усторйства (reset).");
                        break;
                    case COMMAND_44:
                        emit writeStatusBar("Единицы измерения записаны", 3000);
                        break;
                    case SET_UNIQUE_ID:
                        tmp = rxLongFrame->data[2]; tmp <<= 8;
                        tmp |= rxLongFrame->data[3]; tmp <<= 8;
                        tmp |= rxLongFrame->data[4];
                        emit transactionComplete(rxLongFrame->command, tmp);
                        break;
                    case ERASE_APP_FLASH:
                        if ((rxLongFrame->data[1]) != 0) { // check ststus byte_1
                            this->devErrorHandler(rxLongFrame->data[1], rxLongFrame->command);
                        }
                        else {
                            emit eraseComplete();
                        }
                        break;
                    case WRITE_FLASH_FIRST:
                    case WRITE_FLASH_NEXT:
                        emit transactionComplete(rxLongFrame->command, 0);
                        break;
                    case CHECK_HASH_SUM:
                        tmp = rxLongFrame->data[2]; tmp <<= 8;
                        tmp |= rxLongFrame->data[3]; tmp <<= 8;
                        tmp |= rxLongFrame->data[4]; tmp <<= 8;
                        tmp |= rxLongFrame->data[5];
                        emit transactionComplete(rxLongFrame->command, tmp);
                        break;
                    case CHECK_MEMORY:
                        emit transactionComplete(rxLongFrame->command, rxLongFrame->data[2]);
                        break;
                    case CLEAR_CONFIG:
                        QMessageBox::information(mainWidget, tr("Подтверждение"), "Установлены заводские настройки.");
                        break;
                    default:
                        break;
                }
                break;
            default:
                // Wrong packet
                qDebug("Wrong packet!");
                break;
        }
    }
    checkPacketQueue();
    emit showPacketSize(packetQueue.size());
}

/**
 * @brief HartPro::linkErrorHandler. Handles data link and command errors.
 * @param err_code - Error code
 * @param cmd_code - Command code (opcode)
 */
void HartPro::linkErrorHandler(quint8 err_code)
{
    QString errMessage;

    err_code &= ~0x80;

    switch (err_code)
    {
    case BUFF_OVER_ERR:
        errMessage = "(Переполнен буфер приемника)";
        break;
    case CHECKSUM_ERR:
        errMessage = "(Ошибка контрольной суммы)";
        break;
    case FRAMING_ERR:
        errMessage = "(Ошибка формирования фрейма)";
        break;
    case OVERRUN_ERR:
        errMessage = "(Ошибка переполнения)";
        break;
    case PARITY_ERR:
        errMessage = "(Ошибка по четности)";
        break;
    default:
        errMessage = "(Код ошибки: " + QString::number(err_code)+")";
        break;
    }
    QMessageBox::warning(mainWidget, tr("Ошибка"), "<h4>Ошибка передачи данных</h4>"+errMessage);
}

/**
 * @brief HartPro::cmdRespHandler
 * @param err_code
 * @param cmd_code
 */
void HartPro::cmdRespHandler(quint8 err_code, quint8 cmd_code)
{
    Q_UNUSED(cmd_code)
//    QString errMessage = "";

//            switch (err_code)
//            {
//                case TOO_FEW_BYTES:
//                    errMessage = "Слишком мало байт данных";
//                    break;
//                case ACCESS_LIMITED:
//                    errMessage = "Доступ ограничен";
//                    break;
//                case DEVICE_IS_BUSY:
//                    errMessage = "Устройство занято";
//                    break;
//                case UNSUPPORTED_CMD:
//                    errMessage = "Команда не поддерживается устройством или\n"
//                                 "устройство не находится в нужном режиме.\n"
//                                 "Код команды: "+QString::number(cmd_code);
//                    break;
//                default:
//                    errMessage = "Код ошибки: " + QString::number(err_code);
//                    break;
//            }
//            QMessageBox::information(mainWidget, tr("Ошибка"), "<h4>Ошибка выполнения команды</h4>"+errMessage);

    emit writeStatusBar("Command Response Code: " + QString::number(err_code), 3000);
}

/**
 * @brief HartPro::devErrorHandler. Handles device runtime errors.
 * @param err_code -  Error code
 * @param cmd_code -  Command code (opcode)
 */
void HartPro::devErrorHandler(quint8 err_code, quint8 cmd_code)
{
    QString errMessage;

    switch (cmd_code)
    {
        case ERASE_APP_FLASH:
        case WRITE_FLASH_FIRST:
        case WRITE_FLASH_NEXT:
            if ((err_code & 0x80) != 0x80) {
                switch (err_code)
                {
                case flash_erase_err:
                    errMessage = "Не удалось стереть FLASH";
                    break;
                case flash_write_err:
                    errMessage = "Не удалось записать FLASH";
                    break;
                case eeprom_write_err:
                    errMessage = "Не удалось записать EEPROM";
                    break;
                default:
                    errMessage = "Код ошибки: " + QString::number(err_code);
                    break;
                }
            }
            break;
        default:
            break;
    }
    QMessageBox::warning(mainWidget, tr("Ошибка"), "<h4>Сбой при работе с памятью устройства</h4>"+errMessage);
}

/**
 * @brief HartPro::timerEvent. Handles timer events.
 * @param event.  Timer event
 */
void HartPro::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == responseTimeout) {
        if (responseTimeout != 0) {
            killTimer(responseTimeout);
            responseTimeout = 0;
        }
        isPortBusy = false;

        if (isPollingRun) {
            checkPacketQueue();
        }
        else {
            QMessageBox::information(mainWidget, tr("Сообщение"), "Устройство не отвечает.");
        }
    }
}

/**
 * @brief HartPro::onSetDeviceAddress
 * @param addr
 */
void HartPro::onSetDeviceAddress(quint64 addr)
{
    uniqueAddress = addr;
}

/**
 * @brief HartPro::onClearPacketQueue
 */
void HartPro::onClearPacketQueue()
{
    if (!packetQueue.isEmpty()) {
        packetQueue.clear();
    }
}

bool HartPro::checkPacketQueue()
{
    QByteArray packet;
    static quint8 tmp = 0;

    if (packetQueue.isEmpty()) {
        return false;
    }
    else {
        isPortBusy = true;
        packet.append(packetQueue.dequeue());

        if (packetQueue.size() > 64) {
            packetQueue.clear();
            emit writeStatusBar("Очередь пакетов сброшена", 3000);
        }

        if (packet.at(0) & UNIQUE_ADDR_TYPE) {
            isPollingRun = false;
        }
        else { /* polling */
            tmp = packet.at(1) & 0x3F;
            emit showPolligProgress(tmp);
        }
        responseTimeout = this->startTimer(respTaValue);

        emit sendDataPacket(packet);        
        if (tmp == DEV_MAX_NUMBER-1) {
            tmp = 0;            
            emit searchCompleted();            
        }
    }
    return true;
}
//eof
