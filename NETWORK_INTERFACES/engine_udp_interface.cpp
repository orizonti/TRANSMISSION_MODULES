#include "engine_udp_interface.h"
#include "connection_interface.h"
#include <qhostaddress.h>
#include "debug_output_filter.h"


UDPConnectionEngine::UDPConnectionEngine(QString IPDevice, int Port, QString IPListen, int PortLocal, QObject *parent)
    : ConnectionInterface{parent}
{

    qDebug() << "[ UDP CONNECTION ]" << IPDevice << Port << IPListen << PortLocal;
    connectTo(IPDevice, Port); 
    listenTo(IPListen, PortLocal);
}

UDPConnectionEngine::UDPConnectionEngine(QString IPDevice, int Port,QHostAddress::SpecialAddress IPListen, int PortLocal, QObject *parent)
 : ConnectionInterface(parent)
{
    connectTo(IPDevice, Port); 
    listenTo(IPListen, PortLocal);
    qDebug() << "[ UDP CONNECTION ]" << IPDevice << Port << IPListen << PortLocal;
}

UDPConnectionEngine::UDPConnectionEngine(QObject* parent) : ConnectionInterface(parent)
{
    qDebug() << "[ UDP CONNECTION NEED SET SOCKET]" ;
}

void UDPConnectionEngine::connectTo(QString IPDevice, int Port) 
{
    if(isConnected()) slotCloseConnection();

    if(Socket == nullptr) { Socket = new QUdpSocket(this); 
                            Socket->open(QIODevice::ReadWrite); 
                            QObject::connect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
                          }

    PortRemote = Port; IPRemote = IPDevice;
    qDebug() << "[ UDP SOCKET SET REMOTE ]" << IPRemote << Port;
};

void  UDPConnectionEngine::listenTo(QString IPHost, int Port)
{
    if(Socket != nullptr) 
    { 
        QObject::disconnect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
                     delete Socket; Socket = nullptr; 
    }; 
       Socket = new QUdpSocket(this); 
       Socket->open(QIODevice::ReadWrite); 
       Socket->bind(QHostAddress(IPHost), Port); 
       //Socket->bind(QHostAddress::Any, Port); 
       Socket->flush();

    if(Socket->isOpen()) 
    {
       QObject::connect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
       qDebug() << "[ UDP SOCKET OPENED ]" << IPHost << Port;
    }
    else 
       qDebug() << "[ UDP SOCKET NOT OPENED ]" << IPHost << Port;
}

void  UDPConnectionEngine::listenTo(QHostAddress::SpecialAddress IPHost, int Port)
{
    if(Socket != nullptr) 
    { 
        QObject::disconnect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
                     delete Socket; Socket = nullptr; 
    }; 
       Socket = new QUdpSocket(this); 
       Socket->open(QIODevice::ReadWrite); 
       Socket->bind(IPHost, Port); 
       //Socket->bind(QHostAddress::Any, Port); 
       Socket->flush();

    if(Socket->isOpen()) 
    {
       QObject::connect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
       qDebug() << "[ UDP SOCKET OPENED ]" << IPHost << Port;
    }
    else 
       qDebug() << "[ UDP SOCKET NOT OPEN ]" << IPHost << Port;
}

UDPConnectionEngine::~UDPConnectionEngine()
{
 qDebug() << "DELETE UDP INTERFACE";
 delete Socket;
}

void UDPConnectionEngine::slotReadData()
{
   if(MessageStorage == nullptr) return;
   if(Socket->bytesAvailable() < MessageStorage->getMinMessageSize() ) return;

      auto Datagram = Socket->receiveDatagram();
      DataCounter += Datagram.data().size();

      MessageStorage->appendData((uint8_t*)Datagram.data().data(), Datagram.data().size());

   if(MessageStorage->isMessageAvailable()) emit signalMessageAvailable();

   if(Socket->bytesAvailable() > 24) slotReadData();
}


void UDPConnectionEngine::slotSendMessage(const QByteArray& Command, uint16_t Param)
{
 //qDebug() << OutputFilter::Filter(10) << "SEND COMMAND: " << QString(Command.toHex()) << "SIZE: " << Command.size() << "SOCKET: " << IPRemote << PortRemote;
 Socket->writeDatagram(Command,QHostAddress(IPRemote),PortRemote); 
}

void UDPConnectionEngine::slotSendMessage(const char* Command, int size, uint16_t Param)
{
    Socket->writeDatagram(Command,size,QHostAddress(IPRemote), PortRemote);
}



void UDPConnectionEngine::slotCloseConnection()
{
    Connected = false; 
}


bool UDPConnectionEngine::isConnected() { return Connected;}

void UDPConnectionEngine::tryConnectConstantly(QString Address, int Port)
{
   Connected = false; 

   MessageStorage->reset(); connectTo(Address, Port); slotCheckConnection();

   QTimer::singleShot(2000, this, SLOT(slotCheckConnection()));
}

void UDPConnectionEngine::slotCheckConnection()
{
  if(MessageStorage->isMessageAvailable()) 
  {
  qDebug() << "CONNECTED TO : " << IPRemote << ":" << PortRemote; 
  Connected = true; 
  }

  qDebug() << "TRY TO CONNECT TO : " << IPRemote << ":" << PortRemote; 
  QTimer::singleShot(2000, this, SLOT(slotCheckConnection()));
}


bool UDPConnectionEngine::isMessageAvailable() { return MessageStorage->isMessageAvailable(); }
