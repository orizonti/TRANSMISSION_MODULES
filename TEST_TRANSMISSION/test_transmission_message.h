#ifndef TEST_TRANSMISSION_MESSAGE_H
#define TEST_TRANSMISSION_MESSAGE_H
#include "engine_type_register.h"

#include "message_command_structures.h"
#include "message_ring_buffer.h"
#include "message_dispatcher_generic.h"

#include "engine_udp_interface.h"
#include <QDebug>

template<typename T_MESSAGE>
class TestMessageTransmission 
{
  static constexpr int MinSize = TypeRegister<>::GetMinTypeSize<sizeof(MESSAGE_HEADER_GENERIC)>();
  using MessageType    = MessageGeneric<void*, MESSAGE_HEADER_GENERIC>;
  using BufferType     = RingBufferGeneric<MESSAGE_HEADER_GENERIC, TypeRegister<>::GetMinTypeSize<MinSize>(), 20,IteratorMode::Continous>; 
  using DispatcherType = MessageDispatcher<MESSAGE_HEADER_GENERIC, BufferType>;
  public:

  TestMessageTransmission(QString remote_ip, int port, QString local_ip, int port2 )
  {
            Connection = std::make_shared<UDPConnectionEngine>();
            RingBuffer = std::make_shared<BufferType>();
            Dispatcher = std::make_shared<DispatcherType>();
   *Connection | RingBuffer | Dispatcher;

    Connection->connectTo(remote_ip,port );
    Connection->listenTo ( local_ip,port2);

    Dispatcher->AppendCallback<T> ( [](MessageType& Message)
    {
     auto data = DispatcherType::ExtractData<T>(&Message); 
     ConnectionValid = (*data == CommandTest);
     
     qDebug() << "GET REQUEST: " << TypeRegister<T>::GetTypeName() << data->Param << "[ VALID ] " << ConnectionValid;
    });
  }

  MessageGeneric<T_MESSAGE, MESSAGE_HEADER_GENERIC> CommandTest;
  std::shared_ptr<UDPConnectionEngine>     Connection ;
  std::shared_ptr<MessageStorageInterface> RingBuffer ;
  std::shared_ptr<DispatcherType>          Dispatcher ;

  bool ConnectionValid = false;
  bool    isNodeSource = false;

  template<typename T> void sendTestMessage()
  {
    CommandTest.Param++;
    qDebug() << "[ SEND MESSAGE ]" << TypeRegister<T>::GetTypeName() << CommandTest.Param;
             
    Connection->slotSendMessage((const char*)(&CommandTest), CommandTest.GetSizeMessage(),0);
  }
};

#endif