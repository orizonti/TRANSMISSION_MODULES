#ifndef MessageGeneric_H
#define MessageGeneric_H

#include <QDataStream>
#include <QIODevice>

#include <algorithm>
#include <set>
#include "engine_type_register.h"
#include <QByteArray>
#include <array>

template<typename T> concept HAS_DATA_SIZE_FIELD = requires(T t) { t.DATA_SIZE; };

template<typename T> concept HAS_MAX_SIZE = requires(T t) 
{ 
  { T::getMaxSize() } -> std::convertible_to<int>;
  { T::getMaxSizeStatic() } -> std::convertible_to<int>;
};

template<typename T> concept HAS_ANY_MESSAGE_FUNC = requires(T H) 
{ 
   { H.getMessageSize() } -> std::convertible_to<int>; 
   { H.isValid() } -> std::convertible_to<bool> ; 
   { T::getHeaderMarker() } -> std::convertible_to<int>;
};


template<class HEADER_TYPE>
concept HEADER_GENERIC = requires(HEADER_TYPE H) 
{ 
   requires !HAS_DATA_SIZE_FIELD<HEADER_TYPE>;
   requires !HAS_MAX_SIZE<HEADER_TYPE>;
   requires  HAS_ANY_MESSAGE_FUNC<HEADER_TYPE>;

};

template<class HEADER_TYPE>
concept HEADER_VARIABLE = requires(HEADER_TYPE H) 
{
    requires !HAS_DATA_SIZE_FIELD<HEADER_TYPE>;
    requires  HAS_MAX_SIZE<HEADER_TYPE>;
    requires  HAS_ANY_MESSAGE_FUNC<HEADER_TYPE>;
};

template<class HEADER_TYPE>
concept HEADER_STANDART = requires(HEADER_TYPE H) 
{ 
    requires HAS_DATA_SIZE_FIELD<HEADER_TYPE>;
    requires HAS_ANY_MESSAGE_FUNC<HEADER_TYPE>;
};

template<typename T> concept HAS_HEADER_FIELD = requires(T t) { t.HEADER; };
template<typename T> concept HAS_DATA_FIELD = requires(T t)   { t.DATA; };

template<class MESSAGE_TYPE>
concept MESSAGE_STANDART = requires(MESSAGE_TYPE M) 
{ 
    requires HAS_HEADER_FIELD<MESSAGE_TYPE>;
    requires HAS_DATA_FIELD<MESSAGE_TYPE>;
};

template<typename T, typename H> 
class MessageGeneric
{
  public:
    MessageGeneric() 
    { 
      HEADER.MESSAGE_IDENT = TypeRegister<T>::GetTypeID(); 
      qDebug() << "[ CREATE MESSAGE GENERIC ]";
    };
	public:
    H HEADER;
    T DATA; 
  public:
                  bool isMessasge()     { return (HEADER.isValid()); };
                   int GetSize()        { return HEADER.getMessageSize();  };
  constexpr static int GetSizeMessage() { return sizeof(MessageGeneric<T,H>); };
  MessageGeneric<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageGeneric<void*,H>*>(this);  }
};

template<typename T, HEADER_VARIABLE H> 
class MessageGeneric<T,H>
{
  public:
    MessageGeneric()
    { 
      defaultMessage >> DATA;
      HEADER.MESSAGE_IDENT = TypeRegister<T>::GetTypeID(); 
      qDebug() << "[ CREATE MESSAGE GENERIC VARIABLE ] SIZE: " << H::getMaxSizeStatic() 
               << "ID: " << TypeRegister<T>::GetTypeID()
               << "TYPE: " << TypeRegister<T>::GetTypeName();
    };
	public:
    static H HEADER;
    std::array<uint8_t,H::getMaxSizeStatic()> DATA;
  public:
	                bool isMessasge()     { return ((H*)(&DATA))->isValid(); };
                   int GetSize()        { return H::getMaxSize(); };
  constexpr static int GetSizeMessage() { return H::getMaxSize(); };

         void operator>>(T& Rec) { Rec.loadData(DATA); }
  friend void operator>>(T& Src, MessageGeneric<T,H>& Rec) { Src >> Rec.DATA; }

  MessageGeneric<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageGeneric<void*,H>*>(this);  }
  private:
  static T defaultMessage;

};
template<typename T, HEADER_VARIABLE H> H MessageGeneric<T,H>::HEADER;
template<typename T, HEADER_VARIABLE H> T MessageGeneric<T,H>::defaultMessage;

template<typename T, HEADER_STANDART H> 
class MessageGeneric<T,H>
{
  public:
    MessageGeneric()
    { 
      HEADER.DATA_SIZE = sizeof(T);
      HEADER.MESSAGE_IDENT = TypeRegister<T>::GetTypeID();
      qDebug() << "[ CREATE MESSAGE GENERIC STANDART ]";
    };
	public:
    H HEADER;
    T DATA; 
  public:

                  bool isMessasge() { return (HEADER.isValid()); };
                   int GetSize()       { return HEADER.DATA_SIZE + sizeof(H);  };
  constexpr static int GetSizeMessage() { return sizeof(MessageGeneric<T,H>); };

  MessageGeneric<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageGeneric<void*,H>*>(this);  }

};

//==========================================================

#endif 