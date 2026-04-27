#ifndef MESSAGE_STRUCT_GENERIC_H
#define MESSAGE_STRUCT_GENERIC_H
#include <QString>
#include "message_struct_generic.h"

template<typename T, typename H>
class MessageGenericExt : public MessageGeneric<T,H>
{
  public:
    MessageGenericExt();

  friend void operator>>(QDataStream& stream, MessageGenericExt<T,H>& Message);
  friend void operator<<(QDataStream& stream, MessageGenericExt<T,H>& Message);

     QByteArray     toByteArray();
     QByteArray castToByteArray();
           void dumpToByteArray(QByteArray& array);
           void operator>>     (QByteArray& array);
    static QString printDataType() { return TypeRegister<T>::GetTypeName(); };

  template<typename T_PARAM> void setData(const T_PARAM& DATA_PARAMS) { this->DATA = DATA_PARAMS; };
  template<typename T_PARAM> void setData(const T_PARAM& PARAM1, const T_PARAM& PARAM2) { this->DATA.setData(PARAM1, PARAM2);; };

        QString toString();
           bool isMessasge() { return (this->HEADER.isValid()); };

  void operator=(const T& command) { this->DATA = command; };
      MessageGenericExt<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageGenericExt<void*,H>*>(this);  }
  private:
  static QByteArray RawData;
};

template<typename T,typename H> QByteArray MessageGenericExt<T,H>::RawData;

template<typename T,typename H> 
MessageGenericExt<T,H>::MessageGenericExt() 
{
   this->HEADER.MESSAGE_IDENT = TypeRegister<T>::GetTypeID(); 
   qDebug() << "[ CREATE MESSAGE GENERIC ] " << Qt::hex << TypeRegister<T>::GetTypeID() << TypeRegister<T>::GetTypeName() ;
};
template<typename T,typename H> 
 QString MessageGenericExt<T,H>::toString() { return QString("VAL: %1").arg(this->DATA); }

template<typename T,typename H>
void MessageGenericExt<T,H>::dumpToByteArray(QByteArray& RawData)
{
QDataStream out_stream(&RawData, QIODevice::ReadWrite);
            out_stream.setByteOrder(QDataStream::LittleEndian);
            out_stream.setFloatingPointPrecision(QDataStream::DoublePrecision);

out_stream << this->HEADER;
out_stream << this->DATA;
};

template<typename T,typename H>
void MessageGenericExt<T,H>::operator>>(QByteArray& RawData)
{
QDataStream out_stream(&RawData, QIODevice::ReadWrite);
            out_stream.setByteOrder(QDataStream::LittleEndian);
            out_stream.setFloatingPointPrecision(QDataStream::DoublePrecision);

out_stream << this->HEADER;
out_stream << this->DATA;
};

template<typename T,typename H>
QByteArray MessageGenericExt<T,H>::castToByteArray()
{
             RawData.setRawData((char*)this,this->GetSizeMessage());
      return RawData;
};

template<typename T,typename H>
QByteArray MessageGenericExt<T,H>::toByteArray()
{
             RawData.setRawData((char*)this,this->GetSizeMessage());
      return RawData;
};


template<typename T,typename H>
void operator>>(QDataStream& stream, MessageGenericExt<T,H>& Message)
{
  stream >> Message.HEADER; 
  stream >> Message.DATA;
}

template<typename M,typename H> void operator<<(QDataStream& stream, MessageGenericExt<M,H>& Message)
{
  stream << Message.HEADER; 
  stream << Message.DATA;
}



#endif // MESSAGE_STRUCT_ENGINE_H
