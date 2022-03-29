/**
 * @file convert.c
 * @brief this file contain a functions for data converting
 * (c)2018 Sarov
 */
#include "convert.h"
#include "string.h"


/**
 * @brief places float to byte array
 * @param i - integer number or variable
 * @param array - byte array
 */
void IntToByteArray(qint32 i, QByteArray &array)
{
    array.append(static_cast<char>(i>>24));
    array.append(static_cast<char>(i>>16));
    array.append(static_cast<char>(i>>8));
    array.append(static_cast<char>(i>>0));
}

/**
 * @brief places float to byte array
 * @param f - float number or variable
 * @param array - byte array
 */
void FloatToByteArray(float f, QByteArray &array)
{
    int *ptr = reinterpret_cast<int*>(&f);

    array.append(static_cast<char>(*ptr>>24));
    array.append(static_cast<char>(*ptr>>16));
    array.append(static_cast<char>(*ptr>>8));
    array.append(static_cast<char>(*ptr>>0));
}

/**
 * @brief places byte array to float
 * @param array - byte array
 * @param pos - position in the array
 * @retval float representation
 */
float ByteArrayToFloat(const QByteArray &array, quint16 pos)
{
    int tmp = 0;
    float* fp = reinterpret_cast<float*>(&tmp);
    
    tmp |= array.at(pos+0) & 0x000000FF; tmp <<= 8;
    tmp |= array.at(pos+1) & 0x000000FF; tmp <<= 8;
    tmp |= array.at(pos+2) & 0x000000FF; tmp <<= 8;
    tmp |= array.at(pos+3) & 0x000000FF;

    return *fp;
}

/**
 * @brief memcpy in reverse order src
 * @param dst - detination address
 * @param src - source address
 * @param size - size of data to copy
 */
void memcpyRevers(char *dst, char *src, quint32 size)
{
    while(size--) {
        *dst++ = *(src+size);
    }
}
//eof
