#ifndef UART_ENGINE_INTERFACE_H
#define UART_ENGINE_INTERFACE_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QImage>
#include <QElapsedTimer>

#include <QMutex>
#include <QThread>
#include <QByteArray>
#include <QBuffer>
#include <QTimer>
#include "engine_ring_buffer_generic.h"
#include "connection_interface.h"
#include "message_dispatcher_generic.h"
#include "debug_output_filter.h"
#include <QSerialPort>


class UARTConnectionEngine : public ConnectionInterface
{
    Q_OBJECT
public:
  explicit UARTConnectionEngine(QObject *parent = nullptr);
          ~UARTConnectionEngine();

public:
  bool isMessageAvailable() override;
  bool isConnected() override;
  void connectTo(QString IPDevice, int Speed) override;
  void  listenTo(QString IPDevice, int Speed) override;
  void  listenTo(QHostAddress::SpecialAddress IPDevice, int Port) override;
  void tryConnectConstantly(QString Address, int Port) override;

public slots:
  void slotSendMessage(const QByteArray& Command, uint16_t Param = 0) override;
  void slotSendMessage(const char* DataCommand, int size, uint16_t Param = 0) override;

  void slotCheckConnection() override;
  void slotCloseConnection() override;

private slots:
  void slotReadData();
  void slotConnectedToHost();
  void slotConnectionAttempt();
  void slotAcceptConnection();

private:
//=================================================================
QSerialPort Port;
public:

  signals:
  void signalDeviceConnected();
  void signalMessageAvailable();
};


#endif // UART_ENGINE_INTERFACE_H
