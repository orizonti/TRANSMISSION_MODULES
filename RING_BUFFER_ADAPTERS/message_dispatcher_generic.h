#ifndef MESSAGE_DISPATCHER_EXT_H
#define MESSAGE_DISPATCHER_EXT_H

#include "message_command_structures.h"
#include "message_struct_generic.h"
#include "engine_type_register.h"
#include "message_header_generic.h"
#include "message_storage_interface.h"
#include <map>

#include <typeinfo>


template<typename H, typename BufferType>
class MessageDispatcher : public MessageDispatcherInterface
{
public:
    MessageDispatcher() { }
    using MessageCall   = std::function<void (MessageGeneric<void*, H>&)>;

//   void DispatchMessages();
//   void DispatchMessages(std::shared_ptr<MessageStorageInterface> Storage);
//   void DispatchMessages(                MessageStorageInterface& Storage);

   void linkTo(std::shared_ptr<MessageStorageInterface> Storage) 
   { LinkedStorage = std::static_pointer_cast<BufferType>(Storage); };


    template<typename T>
    void AppendCallback(const MessageCall& Call)
    {
       CallList[TypeRegister<T>::TYPE_ID] = Call;
       qDebug() << "APPEND CALLBACK TO : "<< typeid(T).name() << " ID:" << TypeRegister<T>::TYPE_ID;
    }


    template< typename T, typename MessageType> 
    static T* ExtractData(MessageType* Message) { return &(reinterpret_cast<MessageGeneric<T,H>*>(Message)->DATA); }

    int CounterDispatchFail = 0;
    std::map<uint16_t,MessageCall> CallList;

void DispatchMessages()
{

         if(CounterDispatchFail > 5) { LinkedStorage->reset(); CounterDispatchFail=0; return; }
                                   if( LinkedStorage == nullptr) return;
                                   if(!LinkedStorage->isMessageAvailable()) return;
    MessageGeneric<void*,H>& Message = LinkedStorage->takeMessage<H>();

    if(!TypeRegister<>::isTypeRegistered(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "MESSAGE NOT REGISTERED:" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList.contains(Message.HEADER.MESSAGE_IDENT)) CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    {
    CounterDispatchFail++; //qDebug() << "CALLBACK NOT REGISTERED TO MESSAGE:" << Message.HEADER.MESSAGE_IDENT;
    }

    if(LinkedStorage->isMessageAvailable()) DispatchMessages();

}

void DispatchMessages(std::shared_ptr<MessageStorageInterface> Storage)
{
                                   if( Storage == nullptr) return;
                                   if(!Storage->isMessageAvailable()) return;
    MessageGeneric<void*,H>& Message = Storage->takeMessage<H>();

    if(!TypeRegister<>::isTypeRegistered(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "MESSAGE NOT REGISTERED:" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList.contains(Message.HEADER.MESSAGE_IDENT)) CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    qDebug() << "CALLBACK NOT REGISTERED TO MESSAGE:" << Message.HEADER.MESSAGE_IDENT;

    if(Storage->isMessageAvailable()) DispatchMessages(Storage);
}

void DispatchMessages(                MessageStorageInterface& Storage)
{
                                    if(Storage.isMessageAvailable()) return;
    MessageGeneric<void*,H>& Message = Storage.takeMessage<H>();

    if(!TypeRegister<>::isTypeRegistered(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "[ MESSAGE NOT REGISTERED ]" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList.contains(Message.HEADER.MESSAGE_IDENT)) CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    qDebug() << "[ CALLBACK NOT REGISTERED TO MESSAGE ]" << Message.HEADER.MESSAGE_IDENT << " [ IN MAP ]";

    //if(Storage.isMessageAvailable()) DispatchMessages(Storage);
}

};



template<int ID_NUM = 0>
class CommandDispatcherGeneric
{
   public:
   static const int ID{ID_NUM};
   static void dispatchCommand(const QByteArray& Command) 
   {
      qDebug() << "[COMMAND DISPATCHER ] ID: " << ID << "COMMAND NOT REGISTERED";
   };
};


template<typename BufferType>
class MessageDispatcher<MESSAGE_HEADER_GENERIC, BufferType> : public MessageDispatcherInterface
{
public:
    MessageDispatcher() { CallList.resize(TypeRegister<>::GetTypeCount()+1); std::fill(CallList.begin(), CallList.end(), nullptr);}
    using MessageCall   = std::function<void (MessageGeneric<void*, MESSAGE_HEADER_GENERIC>&)>;

   //   void DispatchMessages();
   //   void DispatchMessages(std::shared_ptr<MessageStorageInterface> Storage);
   //   void DispatchMessages(                MessageStorageInterface& Storage);

   void linkTo(std::shared_ptr<MessageStorageInterface> Storage) 
   { LinkedStorage = std::static_pointer_cast<BufferType>(Storage); };


    template<typename T>
    void AppendCallback(const MessageCall& Call)
    {
       CallList[TypeRegister<T>::TYPE_ID] = Call;
       qDebug() << "APPEND CALLBACK TO : "<< typeid(T).name() << " ID:" << TypeRegister<T>::TYPE_ID;
    }

    template< typename T, typename MessageType> 
    static T* ExtractData(MessageType* Message) { return &(reinterpret_cast<MessageGeneric<T,MESSAGE_HEADER_GENERIC>*>(Message)->DATA); 
    }

    std::vector<MessageCall> CallList;

void DispatchMessages()
{
                                   if( LinkedStorage == nullptr) return;
                                   if(!LinkedStorage->isMessageAvailable()) return;
    MessageGeneric<void*,MESSAGE_HEADER_GENERIC>& Message = LinkedStorage->takeMessage<MESSAGE_HEADER_GENERIC>();

    if(!TypeRegister<>::isTypeRegistered(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "MESSAGE NOT REGISTERED:" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList[Message.HEADER.MESSAGE_IDENT] != nullptr && 
                Message.HEADER.MESSAGE_IDENT  <= CallList.size())
       CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    qDebug() << "CALLBACK NOT REGISTERED TO MESSAGE:" << Message.HEADER.MESSAGE_IDENT;

    if(LinkedStorage->isMessageAvailable()) DispatchMessages(LinkedStorage);
}

void DispatchMessages(std::shared_ptr<MessageStorageInterface> Storage)
{
                                   if( Storage == nullptr) return;
                                   if(!Storage->isMessageAvailable()) return;
    MessageGeneric<void*,MESSAGE_HEADER_GENERIC>& Message = Storage->takeMessage<MESSAGE_HEADER_GENERIC>();

    if(!TypeRegister<>::isTypeRegistered(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "MESSAGE NOT REGISTERED:" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList[Message.HEADER.MESSAGE_IDENT] != nullptr && 
                Message.HEADER.MESSAGE_IDENT  <= CallList.size())
       CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    qDebug() << "CALLBACK NOT REGISTERED TO MESSAGE:" << Message.HEADER.MESSAGE_IDENT;

    if(Storage->isMessageAvailable()) DispatchMessages(Storage);
}

void DispatchMessages(                MessageStorageInterface& Storage)
{
                                    if(Storage.isMessageAvailable()) return;
    MessageGeneric<void*,MESSAGE_HEADER_GENERIC>& Message = Storage.takeMessage<MESSAGE_HEADER_GENERIC>();

    if(!TypeRegister<>::isTypeRegistered(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "MESSAGE NOT REGISTERED:" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList[Message.HEADER.MESSAGE_IDENT] != nullptr && 
                Message.HEADER.MESSAGE_IDENT  <= CallList.size())
       CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    qDebug() << "CALLBACK NOT REGISTERED TO MESSAGE:" << Message.HEADER.MESSAGE_IDENT;

    if(Storage.isMessageAvailable()) DispatchMessages(Storage);
}

};
#endif //MESSAGE_DISPATCHER_EXT_H