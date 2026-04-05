#ifndef MESSAGE_HEADER_GENERIC_EXT_H
#define MESSAGE_HEADER_GENERIC_EXT_H
#include <stdint.h>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

#include "message_header_generic.h"

class MESSAGE_HEADER_EXT : public MESSAGE_HEADER_GENERIC
{
  public:

    static QByteArray RawData;
    QByteArray toByteArray()
    {
           RawData.setRawData((const char*)this, sizeof(MESSAGE_HEADER_GENERIC));
    return RawData;
    };

    friend QDataStream& operator<<(QDataStream& stream, MESSAGE_HEADER_EXT& Head)
    {
      stream << Head.HEADER;
      stream << Head.MESSAGE_IDENT;
      stream << Head.DATA_SIZE;
      stream << Head.MESSAGE_NUMBER;
      return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, MESSAGE_HEADER_EXT& Head)
    {
      stream >> Head.HEADER;
      stream >> Head.MESSAGE_IDENT;
      stream >> Head.DATA_SIZE;
      stream >> Head.MESSAGE_NUMBER;
      return stream;
    }
};



#endif //MESSAGE_HEADER_GENERIC_EXT_H