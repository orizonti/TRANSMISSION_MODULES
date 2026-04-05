#ifndef RING_BUFFER_GENERIC_H
#define RING_BUFFER_GENERIC_H


#include <cstring>
#include <typeinfo>

#include "engine_type_register.h"
#include "message_storage_interface.h"
#include "message_iterator_generic.h"
#include "message_struct_generic.h"
#include "message_header_generic.h"

template< typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE = IteratorMode::Chunked> 
class RingBufferGeneric : public MessageStorageInterface
{
    public:
    explicit RingBufferGeneric();
            ~RingBufferGeneric();
	std::size_t  BufferSize = M_SIZE*M_NUM;
            uint8_t* DATA = new uint8_t[M_SIZE*M_NUM];

	MessageIteratorGeneric<H_TYPE,IT_MODE> MessagePointer;
	MessageIteratorGeneric<H_TYPE,IT_MODE> IncommingPointer;

    MessageGeneric<void*,H_TYPE>* CurrentMessage = 0;

    bool isMessageAvailable();
    void appendData(uint8_t* Data, uint16_t Size);
    void reset();
    MessageGeneric<void*,H_TYPE>* takeMessagePtr();
	  MessageGeneric<void*,H_TYPE>& takeMessage();
                            void* takeAnyMessage() override;

              int getMessagesInStore()  override { return MESSAGE_AVAILABLE;}
              int getDataInStore()      override { return DATA_AVAILABLE; };
              int getDataPassed()       override { return DATA_PASSED; };
    constexpr int getMinMessageSize()   override { return M_SIZE; } ; 

    int MESSAGE_PASSED = 0;
    int MESSAGE_AVAILABLE = 0;
    int MAX_MESSAGE_IN_STORE = M_NUM-2;

    int DATA_PASSED = 0;
    int DATA_AVAILABLE = 0;

};

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::RingBufferGeneric() :
  MessagePointer  (DATA,M_SIZE,M_NUM),  IncommingPointer(DATA,M_SIZE,M_NUM)  
{
  CurrentMessage = MessagePointer.GetMessagePtr();
}

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
void RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::reset()
{
      MessagePointer.ResetIterator();
    IncommingPointer.ResetIterator();
    MESSAGE_AVAILABLE = 0;
}

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::~RingBufferGeneric() { delete DATA; }

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
void RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::appendData(uint8_t* Data, uint16_t Size)
{
   DATA_PASSED += Size;
   IncommingPointer.LoadData(Data, Size); 
   MessagePointer.RangeLimits = IncommingPointer.RangeLimits;
   MESSAGE_AVAILABLE = IncommingPointer.MessageNumber;

   //qDebug() << "[ MESSAGE IN STORE ]" << MESSAGE_AVAILABLE << "[ PASSED ]" << MESSAGE_PASSED; 

   if(MESSAGE_AVAILABLE > 8) MessagePointer++;

   if(MessageStorageInterface::LinkedDispatcher) this->LinkedDispatcher->DispatchMessages();
}

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE> 
MessageGeneric<void*,H_TYPE>* RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::takeMessagePtr()
{
                     if(IncommingPointer.MessageNumber <= 0) return CurrentMessage;

         CurrentMessage = MessagePointer.GetMessagePtr(); MessagePointer++; MESSAGE_PASSED++;

                        IncommingPointer.MessageNumber--; 
    MESSAGE_AVAILABLE = IncommingPointer.MessageNumber; 
       DATA_AVAILABLE = DATA_PASSED - CurrentMessage->GetSize();

  return CurrentMessage;
}

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
void* RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::takeAnyMessage()
{
                     if(IncommingPointer.MessageNumber <= 0) return CurrentMessage;

         CurrentMessage = MessagePointer.GetMessagePtr(); MessagePointer++; MESSAGE_PASSED++;

                        IncommingPointer.MessageNumber--; 
    MESSAGE_AVAILABLE = IncommingPointer.MessageNumber; 
       DATA_AVAILABLE = DATA_PASSED - CurrentMessage->GetSize();

  return reinterpret_cast<void*>(CurrentMessage);
}

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
MessageGeneric<void*,H_TYPE>& RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::takeMessage()
{

      auto& Message = *MessagePointer; CurrentMessage = &Message; if(IncommingPointer.MessageNumber <= 0) return Message;
                       MessagePointer++; MESSAGE_PASSED++;
  MESSAGE_AVAILABLE  = MessagePointer.StepsTo(IncommingPointer); 

                       IncommingPointer.MessageNumber--; 
    MESSAGE_AVAILABLE = IncommingPointer.MessageNumber; 
       DATA_AVAILABLE = DATA_PASSED - CurrentMessage->GetSize();

  return Message;
}

template<typename H_TYPE, size_t M_SIZE, size_t M_NUM, IteratorMode IT_MODE>
bool RingBufferGeneric<H_TYPE,M_SIZE,M_NUM,IT_MODE>::isMessageAvailable()
{
  return IncommingPointer.MessageNumber > 0;
}


#endif //RING_BUFFER_GENERIC_H

