#ifndef HART_H
#define HART_H

#include <QObject>

#define PREAMBLE_SIZE       5

#define DATA_CHUNCK_SIZE    252 // must be a multiple of 4

#define MESSAGE_SIZE        24
#define TAG_SIZE            6
#define DESCRIPTOR_SIZE     12
#define DATE_SIZE           3
#define LONGTAG_SIZE        32

#define DEV_MAX_NUMBER      64

/* HART Frame Type */
#define STX                 static_cast<quint8>(0x02)

/* HART Address Type */
#define POLING_ADDR_TYPE    static_cast<quint8>(0x00)       // 0 Poling (1 Byte) Address
#define UNIQUE_ADDR_TYPE    static_cast<quint8>(0x80)       // 1 Unique (5 Byte) Address

/* HART Master Type */
#define PRIMARY_MASTER      static_cast<quint8>(0x80)
#define SECONDARY_MASTER    static_cast<quint8>(0x00)

/* Flash and EEPROM errors */
typedef enum{
    no_error = 0,
    flash_erase_err,
    flash_write_err,
    eeprom_write_err
}LoadError_t;

/* Extended device type codes */
typedef enum{
    v318i = 0xE42D,
    dd85 = 0xE42E
}DevType_t;

/* Manufacturer codes */
typedef enum{
    Rosemount = 0x26,
    Alcont = 0x6116
}Vendor_t;

/*  */
typedef enum{
    AutomationDevice = 1,               // 1  - Process Automation Device
    DiscreteDevice,                     // 2  - Discrete Device
    Hybrid,                             // 3  - Hybrid: Process Automation + Discrete
    I_O_System,                         // 4  - I/O System
    DiscreteAdapter = 14,               // 14 - Discrete Adapter
    WirelessAutomationDevice = 129,     // 129 - WirelessHART Process Automation Device
    WirelessDiscreteDevice,             // 130 - WirelessHART Discrete Device
    WirelessHybrid,                     // 131 - WirelessHART Hybrid: Process Automation + Discrete
    WirelessGateway,                    // 132 - WirelessHART Gateway
    WirelessAccessPoint = 140,          // 140 - WirelessHART Access Point
    WirelessProcessAdapter,             // 141 - WirelessHART Process Adapter
    WirelessDiscreteAdapter,            // 142 - WirelessHART Discrete Adapter
    WirelessEnable = 144                // 144 - WirelessHART-Enable Handheld/Portable Maintenance Tool
}Profile_t;

/* HART Protocol Commands */
typedef enum{
    /* HART universal commands */
    COMMAND_0 = 0,                  // 0 - Reads the sensor unique ID
    COMMAND_1,                      // 1 - Reads value of the primary variable
    COMMAND_2,                      // 2 - Reads current and percent of range
    COMMAND_3,                      // 3 - Reads value of the current of the PV, and up to four predefined Dynamic Variables
    COMMAND_6 = 6,                  // 6 - Writes the address to the field device
    COMMAND_7,                      // 7 - Read Loop Configuration
    COMMAND_8,                      // 8 - Read Dynamic Variable Classifications
    COMMAND_9,                      // 9 - Read Device Variables with Status
    COMMAND_11 = 11,                // 11 - Reads Unique Identifier Associated with Tag
    COMMAND_12,                     // 12 - Reads a message in the device
    COMMAND_13,                     // 13 - Reads the Tag, Descriptor, and Date contained in the device
    COMMAND_14,                     // 14 - Reads the sensor information by primary variable
    COMMAND_15,                     // 15 - Reads the signal information on primary variable
    COMMAND_16,                     // 16 - Reads the Assembly Number belonging to this device
    COMMAND_17,                     // 17 - Writes a message to the device
    COMMAND_18,                     // 18 - Writes the Tag, Descriptor, and Date in the device
    COMMAND_19,                     // 19 - Writes Assembly Number to Device
    COMMAND_20,                     // 20 - Reads long Tag
    COMMAND_21,                     // 21 - Reads Unique Identifier Associated with Tag
    COMMAND_22,                     // 22 - Writes long Tag

    /* HART common commands */
    COMMAND_33 = 33,                // 33 - Reads the sensor variables
    COMMAND_34,                     // 34 - Writes damping value for the primary variable
    COMMAND_35,                     // 35 - Writes range value for the primary variable
    COMMAND_36,                     // 36 - Upper range value of the primary variable
    COMMAND_37,                     // 37 - Lower range value of the primary variable
    COMMAND_38,                     // 38 - Resets (sets to 'zero') Configuration Change Flag
    COMMAND_39,                     // 39 - Recording from shadow RAM to non-volatile memory or vice versa
    COMMAND_40,                     // 40 - Set device in the fixed current mode of the PV
    COMMAND_41,                     // 41 - Initializes the device self-test function
    COMMAND_42,                     // 42 - Performs Device Reset
    COMMAND_43,                     // 43 - Adjusts Primary variable to zero of the scale
    COMMAND_44,                     // 44 - Writes units of measure of the primary variable
    COMMAND_45,                     // 45 - DAC zero adjustment
    COMMAND_46,                     // 46 - DAC Gain adjustment
    COMMAND_47,                     // 47 - Writes function for the analog output of the device by the Primary Variable
    COMMAND_48,                     // 48 - Read Additional Device Status

    /* Non-Public commands */
    ERASE_APP_FLASH = 122,          // 122 - Non-Public command Erase flash memory of the application
    WRITE_FLASH_FIRST,              // 123 - Non-Public command for start writing to the device flash memory
    WRITE_FLASH_NEXT,               // 124 - Non-Public command Wrire next data chunk to the device flash memory
    START_APPLICATION,              // 125 - Non-Public command Start device application from loader
    SET_UNIQUE_ID,                  // 126 - Non-Public command Set device unique identificator

    /* HART device specific commands */
    CHECK_HASH_SUM = 230,           // 230 - Check hash sum of the writen app memory
    CHECK_MEMORY,                   // 231 - Check device memory
    CLEAR_CONFIG,                   // 232 - Clear configuration data in the EEPROM (Return to the factory settings)
    STOP_CMD    // for manager only
}HartCommand_t;

/* Data transmission errors */
typedef enum{
    // 0x01 - not defined
    BUFF_OVER_ERR = 0x02,
    // 0x04 - reserved
    CHECKSUM_ERR = 0x08,
    FRAMING_ERR = 0x10,
    OVERRUN_ERR = 0x20,
    PARITY_ERR = 0x40,
    TRANSMIT_ERR = 0x80
}TxError_t;

/* Command response codes (Status, first byte) */
typedef enum{
    NO_CMD_ERR = 0,
    //NOT_DEFINED,                    // 1 - not defined
    INVALID_SELECTION = 2,
    PRM_TOO_LARGE,
    PRM_TOO_SMALL,
    TOO_FEW_BYTES,
    SPECIFIC_CMD_ERR,
    WRITE_PROTECT_MODE,
    UPDATE_FAILURE,
    PROC_PRM_TOO_LARGE,
    PROC_PRM_TOO_SMALL,
    WRONG_PRM_CODE,
    WRONG_UNIT_CODE,
    RANGE_OUT_OF_LIMITS,
    UPPER_RANGE_ERR,
    LOWER_RANGE_ERR,
    ACCESS_LIMITED,
    DEVICE_IS_BUSY = 32,
    UNSUPPORTED_CMD = 64,
}CmdError_t;

/* Structure of address for HART long frame */
#pragma pack(push, 1)
typedef struct{
    quint8 dev_type_0;              // Expanded Device Type code
    quint8 dev_type_1;
    quint8 dev_id_0;                // Slave device id most
    quint8 dev_id_1;                // Slave device id secondary
    quint8 dev_id_2;                // Slave device id least
}LongAddress_t;
#pragma pack(pop)

/* Structure of short HART-frame */
#pragma pack(push, 1)
typedef struct{
    quint8 delimeter;               // Start symbol in frame
    quint8 address;                 // Address in frame
    quint8 command;                 // Command in frame
    quint8 data_len;                // Lenght of data field in frame
    quint8 data[64];                // Data field in frame
}HartShortFrame_t;
#pragma pack(pop)

/* Structure of long HART-frame */
#pragma pack(push, 1)
typedef struct{
    quint8 delimeter;               // Start symbol in long frame
    LongAddress_t address;          // Address in long frame
    quint8 command;                 // Command in long frame
    quint8 data_len;                // Lenght of data field in long frame
    quint8 data[254];               // Data field in long frame
}HartLongFrame_t;
#pragma pack(pop)

/*
#pragma pack(push, 1)
typedef struct{
    quint8 extention;
    DevType_t deviceType;
    quint8 reqPreaNum;
    quint8 univCmdRev;
    quint8 specCmdRev;
    quint8 swRevlevel;
    quint8 hwRevlevel;
    quint8 flags;
    quint8 deviceID[3];
    quint8 respPreaNum;
    quint8 deviceVarNum;
    quint16 confChangeCnt;
    quint8 extDevStaus;
    Vendor_t vendorCode;
    Vendor_t distribCode;
    Profile_t devProfile;
}cmd0Template_t;
#pragma pack(pop)
 */

#endif // HART_H
//eof
