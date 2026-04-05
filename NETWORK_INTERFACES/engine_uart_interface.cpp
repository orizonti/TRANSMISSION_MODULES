#include "engine_uart_interface.h"
#include <type_traits>


using MessageVoid = MessageGeneric<void*, MESSAGE_HEADER_GENERIC>;
UARTConnectionEngine::UARTConnectionEngine(QObject *parent)
    : ConnectionInterface(parent)
{

 Port.setBaudRate(115200); 
 Port.setDataBits(QSerialPort::Data8);
 Port.setParity(QSerialPort::NoParity); 
 Port.setStopBits(QSerialPort::OneStop);
 Port.setFlowControl(QSerialPort::NoFlowControl);

 QObject::connect(&Port,SIGNAL(readyRead()),this,SLOT(SlotReadData()));



 //QTimer::singleShot(1500,this,&ChillerInterface::SlotCheckConnection);
}

UARTConnectionEngine::~UARTConnectionEngine() 
{ 
}

//================================================================
void UARTConnectionEngine::connectTo(QString address, int Speed)
{
    if(Port.isOpen() && Port.portName() == address ) return;
    if(Port.isOpen())   Port.close();
 
    Port.setBaudRate(Speed); 
    Port.setPortName(address); 
    Port.open(QSerialPort::ReadWrite);
    if(Port.isOpen()) qDebug() << address << "[ OPENED ]" << "[ SPEED ]" << Speed;

    Port.write(QString("O\r").toUtf8());
    Port.write(QString("E\r").toUtf8());
    Port.write(QString("S6\r").toUtf8());
    Port.write(QString("t12324545\r\n").toUtf8());
}


void UARTConnectionEngine::slotReadData()
{
   if(!MessageStorage) return;
   //if(Port.bytesAvailable() < MessageStorage->getMinMessageSize()) return;

   int bytes_available = Port.bytesAvailable();

      MessageStorage->appendData((uint8_t*)Port.readAll().data(), bytes_available);
   if(MessageStorage->isMessageAvailable()) { emit signalMessageAvailable(); }

}


void UARTConnectionEngine::slotSendMessage(const QByteArray& Command, uint16_t Param)
{
   //qDebug() << OutputFilter::Filter(1) << "SEND COMMAND: " << QString(Command.toHex());
   qDebug() << "SEND COMMAND: " << QString(Command.toHex());
    Port.write(QString("t12324545...[CR]").toUtf8());
   //if(!Port.isOpen()) return; Port.write(Command);
}

void UARTConnectionEngine::slotSendMessage(const char* DataCommand, int size, uint16_t Param)
{
   qDebug() << "SEND COMMAND: " << QString(QByteArray(DataCommand,size).toHex());
    Port.write(QString("t12324545...[CR]").toUtf8());
   //if(!Port.isOpen()) return; Port.write(DataCommand,size);
}



bool UARTConnectionEngine::isConnected()
{
    if(Port.isOpen()) return true;

    return true;
}

void UARTConnectionEngine::tryConnectConstantly(QString Address, int Port)
{
}

void UARTConnectionEngine::slotConnectionAttempt()
{
}

void UARTConnectionEngine::listenTo(QString address, int Port)
{
}

void UARTConnectionEngine::listenTo(QHostAddress::SpecialAddress address, int Port)
{
}

void UARTConnectionEngine::slotCloseConnection()
{
}

void UARTConnectionEngine::slotCheckConnection() { }

void UARTConnectionEngine::slotAcceptConnection()
{
}

void UARTConnectionEngine::slotConnectedToHost()
{
}

bool UARTConnectionEngine::isMessageAvailable() { return MessageStorage->isMessageAvailable(); }