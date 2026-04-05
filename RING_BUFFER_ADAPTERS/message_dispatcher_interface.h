#ifndef MESSAGE_DISPATCHER_INTERFACE_H
#define MESSAGE_DISPATCHER_INTERFACE_H
#include <memory>


class MessageStorageInterface;

class MessageDispatcherInterface
{
   public:
   virtual void DispatchMessages() = 0;
   virtual void DispatchMessages(std::shared_ptr<MessageStorageInterface> Storage) = 0;
   virtual void DispatchMessages(MessageStorageInterface& Storage) = 0;

   void linkTo   (std::shared_ptr<MessageStorageInterface> Storage) {   LinkedStorage = Storage;};
   void operator|(std::shared_ptr<MessageStorageInterface> Storage) {      this->linkTo(Storage); }

   friend void operator>>(std::shared_ptr<MessageStorageInterface> Storage, MessageDispatcherInterface& Dispatcher) 
                                                         { Dispatcher.DispatchMessages(Storage); }

   friend void operator>>(MessageStorageInterface& Storage, MessageDispatcherInterface& Dispatcher) 
                                                         { Dispatcher.DispatchMessages(Storage); }

   std::shared_ptr<MessageStorageInterface> LinkedStorage = nullptr;
};



#endif //MESSAGE_DISPATCHER_INTERFACE_H