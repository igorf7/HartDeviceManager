#ifndef __MYCRC_H
#define __MYCRC_H

#include <QObject>

quint32 calcCRC32(const quint32 word, quint32 crc);
quint32 calcBufCRC32(const quint32 *buf, int len);

#endif // __MYCRC_H
