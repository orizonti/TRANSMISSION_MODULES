#ifndef ENGINE_UDP_INTERFACE_H
#define ENGINE_UDP_INTERFACE_H

#include <QDebug>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTextStream>
#include <QAbstractSocket>
#include <QProcess>
#include <QFile>

#include <connection_interface.h>
#include <QTimer>

#include "message_command_structures.h"

class UDPConnectionEngine : public ConnectionInterface
{
    Q_OBJECT
public:
    UDPConnectionEngine(QString IPDevice, int Port,QString IPListen, int PortLocal, QObject *parent = nullptr);
    UDPConnectionEngine(QString IPDevice, int Port,QHostAddress::SpecialAddress IPListen, int PortLocal, QObject *parent = nullptr);
    explicit UDPConnectionEngine(QObject* parent= nullptr);
    ~UDPConnectionEngine();

    bool isMessageAvailable() override;
    bool isConnected() override;
    void connectTo(QString IPDevice, int Port) override;
    void  listenTo(QString IPHost, int Port) override;
    void  listenTo(QHostAddress::SpecialAddress IPHost, int Port) override;
    void tryConnectConstantly(QString Address, int Port) override;
    int DataCounter = 0;

public slots:
  void slotSendMessage(const QByteArray& Command, uint16_t Param = 0) override;
  void slotSendMessage(const char* Command, int size, uint16_t Param = 0) override;

  void slotCheckConnection() override;
  void slotCloseConnection() override;

private:
   QUdpSocket* Socket = nullptr;
   QTimer timerConnectAttempt;

private slots:
   void slotReadData();

public: 
    QString IPRemote;
        int PortRemote = 7575;
        int PortLocal  = 7575;
       bool Connected  = false;


signals:
void SignalDeviceConnected();
void signalMessageAvailable();

};



#endif // UDPConnectionEngine_H
