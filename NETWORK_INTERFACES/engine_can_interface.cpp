#include "engine_can_interface.h"

#include "message_command_id.h"
#include "debug_output_filter.h"

CANConnectionEngine::CANConnectionEngine(QString Device, QObject *parent) : ConnectionInterface(parent)
{

  QString errorString;
  device = QCanBus::instance()->createDevice( QStringLiteral("socketcan"), Device, &errorString);

  if (!device)                  { qDebug() << "[ CAN DEVICE NOT OPEN ] " << errorString; return; }
       device->connectDevice(); { qDebug() << "[ " << Device << "DEVICE OPENED ] "; }

  QObject::connect(device,&QCanBusDevice::framesReceived, this, &CANConnectionEngine::slotReadData);


      engineSendTest = new CANTestMessage ; engineSendTest->linkTo(this);
  engineDelayMeasure = new CANDelayMeasure; engineDelayMeasure->linkTo(this);

  qDebug() << "==========================";
}

//void CANConnectionEngine::listenTo(QString IDDeviceStr, int IDDevice)
//{
//  DispatcherType<CommandDevice<0>>::appendCall([](CommandDevice<0> Command){}, TypeRegister<CommandDevice<0>>::GetTypeID());
//}

void CANConnectionEngine::printAvailableDevices()
{

  QString errorString;
  const QList<QCanBusDeviceInfo> devices = QCanBus::instance()->availableDevices( QStringLiteral("socketcan"), &errorString);
  if (!errorString.isEmpty()) qDebug() << errorString; 

  for(auto dev: devices) qDebug() << "[ AVAILABLE DEVICE ] " << dev.name();
}

CANConnectionEngine::~CANConnectionEngine() 
{ 
}


void CANConnectionEngine::slotReadData()
{
    while (device->framesAvailable()) 
    {
        const QCanBusFrame frame = device->readFrame();

                                                   QString data;
        if (frame.frameType() == QCanBusFrame::ErrorFrame) data = device->interpretErrorFrame(frame);

        qDebug() << "[ GET FRAME ]" << frame.frameId() << frame.payload().toHex();

        switch(frame.frameId())
        {
//          case TypeRegister<MessageDeviceController>::TYPE_ID: 
//               CommandDispatcherGeneric<TypeRegister<MessageDeviceController>::TYPE_ID>::dispatchCommand(frame.payload());
//            return; 
//          case TypeRegister<MessageDeviceLaserPower>::TYPE_ID: 
//               CommandDispatcherGeneric<TypeRegister<MessageDeviceLaserPower>::TYPE_ID>::dispatchCommand(frame.payload());
//            return; 
//
//          case TypeRegister<MessageDeviceLaserPointer>::TYPE_ID: 
//               CommandDispatcherGeneric<TypeRegister<MessageDeviceLaserPointer>::TYPE_ID>::dispatchCommand(frame.payload());
//            return; 
//
//          case TypeRegister<MessageDeviceFocusator>::TYPE_ID: 
//               CommandDispatcherGeneric<TypeRegister<MessageDeviceFocusator>::TYPE_ID>::dispatchCommand(frame.payload());
//            return; 
//          break;
        }

        emit signalMessageAvailable();
    }
}


void CANConnectionEngine::slotSendMessage(const QByteArray& message, uint16_t IDDevice)
{
    qDebug() << OutputFilter::Filter(100) << "[ CAN ] SEND: " << QString(message.toHex()); 

    QCanBusFrame frame = QCanBusFrame(IDDevice, message);
                 frame.setFrameType(QCanBusFrame::DataFrame);

    device->writeFrame(frame);
}

void CANConnectionEngine::slotSendMessage(const char* DataCommand, int size, uint16_t IDDevice)
{
  QByteArray message(DataCommand,size);

  QCanBusFrame frame = QCanBusFrame(IDDevice, message);
               frame.setFrameType(QCanBusFrame::DataFrame);

  device->writeFrame(frame);
}




bool CANConnectionEngine::isConnected()
{
    return true;
}

void CANConnectionEngine::slotCheckConnection()
{
}

bool CANConnectionEngine::isMessageAvailable() { return false;}
