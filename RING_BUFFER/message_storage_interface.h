#ifndef MESSAGE_STORAGE_INTERFACE_H
#define MESSAGE_STORAGE_INTERFACE_H
#include <stdint.h>
#include "message_dispatcher_interface.h"
#include "message_struct_generic.h"

class MessageStorageInterface
{ 
  public:
    virtual bool isMessageAvailable() = 0;
    virtual void appendData(uint8_t* Data, uint16_t Size) = 0;
    virtual void reset() = 0;

                                  virtual void* takeAnyMessage() = 0;
    template<typename H_TYPE> 
    MessageGeneric<void*,H_TYPE>* takeMessagePtr() { return   reinterpret_cast<MessageGeneric<void*,H_TYPE>*>(takeAnyMessage()); }
    template<typename H_TYPE> 
    MessageGeneric<void*,H_TYPE>& takeMessage()    { return *(reinterpret_cast<MessageGeneric<void*,H_TYPE>*>(takeAnyMessage())); }

    virtual int getMessagesInStore() = 0;
    virtual int getDataInStore() = 0; 
    virtual int getDataPassed() = 0; 
    virtual int getMinMessageSize() = 0; 

    std::shared_ptr<MessageDispatcherInterface> LinkedDispatcher = nullptr;

              void linkTo (std::shared_ptr<MessageDispatcherInterface> Dispatcher) { LinkedDispatcher = Dispatcher; };
    friend void operator| (std::shared_ptr<MessageStorageInterface> Storage, 
                           std::shared_ptr<MessageDispatcherInterface> Dispatcher) 
                           {
                               Storage->linkTo(Dispatcher);
                            Dispatcher->linkTo(Storage);
                          };
};

#endif 

