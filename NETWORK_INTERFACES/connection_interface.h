#ifndef CONNECTION_INTERFACE_H
#define CONNECTION_INTERFACE_H
#include <QObject>
#include <QHostAddress>
#include <qhostaddress.h>
#include "message_storage_interface.h"

class ConnectionInterface : public QObject
{
  //Q_OBJECT
  public:
  ConnectionInterface(QObject* parent = nullptr) : QObject(parent) {}; 
  virtual bool isMessageAvailable() = 0;
  virtual bool isConnected() = 0;
  virtual void connectTo(QString IPDevice, int Port) = 0;
  virtual void  listenTo(QString IPDevice, int Port) = 0;
  virtual void  listenTo(QHostAddress::SpecialAddress IPDevice, int Port) = 0;
  virtual void tryConnectConstantly(QString address, int Port) = 0;

  public slots:
  virtual void slotSendMessage(const QByteArray& ArrayCommand, uint16_t Param = 0) = 0;
  virtual void slotSendMessage(const char* DataCommand, int size, uint16_t Param = 0) = 0 ;

  virtual void slotCheckConnection() = 0;
  virtual void slotCloseConnection() = 0;

  std::shared_ptr<MessageStorageInterface> MessageStorage = nullptr;
  void linkTo(std::shared_ptr<MessageStorageInterface> Storage) { MessageStorage = Storage; };
  std::shared_ptr<MessageStorageInterface> operator|(std::shared_ptr<MessageStorageInterface> Storage) 
  { linkTo(Storage); return Storage;}
};



#endif //CONNECTION_INTERFACE_H
