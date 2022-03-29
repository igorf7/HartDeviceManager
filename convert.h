/**
 * @brief convert.h - header file for convert.c
 *        (c)2018 Sarov
 */
#ifndef __CONVERT_H
#define __CONVERT_H

#include <QObject>

void IntToByteArray(qint32 i, QByteArray &array);
void FloatToByteArray(float f, QByteArray &array);
float ByteArrayToFloat(const QByteArray &array, quint16 pos);
void memcpyRevers(char* dst, char* src, quint32 size);
#endif
//eof
