#ifndef MESSAGE_HEADER_GENERIC_H
#define MESSAGE_HEADER_GENERIC_H
#include <stdint.h>
#include <QByteArray>
#include <QDataStream>

class MESSAGE_HEADER_GENERIC
{
  public:
    uint16_t HEADER = 0x8220; 
    uint16_t MESSAGE_IDENT = 1; 
    uint32_t DATA_SIZE  = 1; 
    uint32_t MESSAGE_NUMBER = 0x0; 

                  bool isValid()         { return HEADER == 0x8220; }
  static constexpr int getHeaderSize()   { return sizeof(MESSAGE_HEADER_GENERIC);};
                   int getMessageSize()  { return sizeof(MESSAGE_HEADER_GENERIC) + DATA_SIZE; }
        static uint8_t getHeaderMarker() { MESSAGE_HEADER_GENERIC MESSAGE; return ((uint8_t*) &(MESSAGE.HEADER))[0]; }
                        uint16_t getID() { return MESSAGE_IDENT; }

};



#endif //MESSAGE_HEADER_GENERIC_H