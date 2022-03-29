#include "mycrc.h"

/**
 * @brief calcCRC32 - compute CRC32 checksum
 * @param word - input data
 * @param crc - initialization value
 * @return result of the compute CRC32
 */
quint32 calcCRC32(const quint32 word, quint32 crc)
{
    qint32 i;

    crc = crc ^ word;
    for (i = 0; i < 32; ++i) {
        if (crc & 0x80000000)
            crc = (crc << 1) ^ 0x04C11DB7; // Polynomial used in STM32
        else
            crc = (crc << 1);
    }
    return crc;
}

/**
 * @brief calcBufCRC32 - compute CRC32 checksum for array of 32bit words
 * @param buf - array of 32bit words
 * @param len - size of array
 * @return result of the compute CRC32
 */
quint32 calcBufCRC32(const quint32 *buf, int len)
{
    quint32 crc = 0xFFFFFFFF;
    quint32 i;
    while(len-- > 0) {
        crc = crc ^ *buf++;
        for (i = 0; i < 32; ++i) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ 0x04C11DB7; // Polynomial used in STM32
            }
            else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}
//eof
