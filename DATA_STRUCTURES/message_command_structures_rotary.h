#pragma once
#include <stdint.h>
#include <string>
#include <sstream>
#include <QDataStream>
#include <QIODevice>
#include <QByteArray>
#include <QDebug>
#include "message_command_structures_intermediate.h"

struct TimeStamp
{
  uint32_t sec = 0x0;  // количество секунд прошедшее с момента запуска
  uint32_t nsec =0x0; // количество наносекунд прошедшее с последней секунды
};

struct SystemState
{
   uint8_t version; // версия протокола
   uint8_t state;   // байт состояния системы
   uint8_t drives;  // количество приводов
   uint8_t temp;    // температура вычислителя в градусах Цельсия
   uint8_t temp1_0; // температура приводов в градусах Цельсия
   uint8_t temp1_1; // температура приводов в градусах Цельсия
   uint8_t temp2_0; // температура оптических клиньев в градусах Цельсия
   uint8_t temp2_1; // температура оптических клиньев в градусах Цельсия

                   void operator=(SystemState& new_state);
   static constexpr int getSize() { return 8;};
};

#pragma pack(1)
struct alignas(1) DriveRX
{
   uint8_t status   = 0;   
   uint8_t mode     = 0x1;  // режим управление
   uint8_t reserve  = 0;  
   uint8_t digital  = 0;  // значение на цифровых входах
    double position = 0;
    double velocity = 0;
     float current  = 0;  // статорный ток на оси q системы координат Парка в амперах
     float voltage  = 0;  // напряжение цепи звена постоянного тока в вольтах
  uint16_t analog1  = 0;  // значение на входе АЦП 1
  uint16_t analog2  = 0;  // значение на входе АЦП 2

  friend QDataStream& operator<<(QDataStream& stream, DriveRX& src)
  {
  stream << src.status      
         << src.mode       
         << src.reserve    
         << src.digital    
         << src.position 
         << src.velocity; 
  stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
  stream << src.current   
         << src.voltage;  
  stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
  stream << src.analog1   
         << src.analog2; return stream;
  };
};

class alignas(1)  ControlRX
{
  public:
  DriveRX value0;   
  DriveRX value1;   
                  void operator=(ControlRX& state);
  static constexpr int getSize() { return sizeof(ControlRX);};
  friend void operator<<(StateRotaryControl& Receiver, ControlRX& src)
  {
    Receiver.Engine1.Position = src.value0.position;
    Receiver.Engine2.Position = src.value1.position;

    Receiver.Engine1.Velocity = src.value0.velocity;
    Receiver.Engine2.Velocity = src.value1.velocity;

    Receiver.Engine1.Mode = src.value0.mode;
    Receiver.Engine2.Mode = src.value1.mode;
  };

  friend void operator<<(QDataStream& stream, ControlRX& src)
  {
    stream << src.value0 << src.value1;
  };

};
#pragma pack()
//=====================================================================================
struct DriveTX
{
   enum class Modes{M_POS = 1, M_VEL = 2};
 //  uint8_t mode      = (uint8_t)Modes::M_VEL;  // целевой режим управление
 //  uint8_t digital   = 0x00;  // значение на цифровых выходах
 // uint16_t reserve   = 0xAA;  // резервное поле данных
 // uint16_t acc_limit = 100;   // лимит по ускорению в градус/сек^2
     uint8_t mode = 0x01;
     uint8_t Param2 = 0x00;
     uint8_t Param3 = 0x00;
     uint8_t Param4 = 0x00;
     uint8_t Param5 = 0xA0;
     uint8_t Param6 = 0x0F;
     uint8_t Param7 = 0x00;
     uint8_t Param8 = 0x00;

    double target    = 0.0;   // задание в зависимости от режима управления

  friend QDataStream& operator>>(QDataStream& stream, DriveTX& rec);
};

class ControlTX
{
  public:
  DriveTX value0; 
  DriveTX value1; 

  QByteArray toByteArray() { return QByteArray((char*)this, sizeof(ControlTX));}

  friend void operator>>(QDataStream& stream, ControlTX& rec);
  friend void operator<<(QDataStream& stream, ControlTX& src);
         void operator>>(QDataStream& stream);

          void operator=(StateRotaryControl setting);
            void setData(StateRotaryControl setting);

  template<typename T> void operator=(QPair<T,T> Pos)     { value0.target = Pos.first; value1.target = Pos.second; };
  template<typename T> void setData(QPair<T,T> Pos)       { value0.target = Pos.first; value1.target = Pos.second; };
                       //void setMode(uint8_t Mode)         { value0.mode = Mode;        value1.mode = Mode;};
                       //void setAccelLimit(uint32_t Limit) { value0.acc_limit = Limit;  value1.acc_limit = Limit;};

                       //void setMode(uint8_t Mode)         { value0.Param1 = Mode;  value1.Param2 = Mode;};
                       void setMode(uint8_t Mode)         
                       { 
                        qDebug() << "ROTARY CONTROL TX SET MODE: " << Mode;
                        value0.mode = Mode; value1.mode= Mode; 
                       };
                       void setAccelLimit(uint32_t Limit) {};
       static constexpr int getSize() { return 28;};
};

struct MESSAGE_HEADER_ROTARY
{
  TimeStamp TIME;                    
  uint16_t  MESSAGE_IDENT = 0xB001;  
  uint16_t  RESERVE       = 0xCCCC;        

                  //bool isValid() {qDebug() <<"ID: " << MESSAGE_IDENT; return MESSAGE_IDENT == 0xA001 || MESSAGE_IDENT == 0xA002; }
                  bool isValid() {return MESSAGE_IDENT == 0xA001 || MESSAGE_IDENT == 0xA002; }
        static uint8_t getHeaderMarker() { MESSAGE_HEADER_ROTARY MESSAGE; return ((uint8_t*) &(MESSAGE.MESSAGE_IDENT))[0]; }
  static constexpr int getHeaderSize() { return 12;};
                   int getMessageSize(); 

          QDataStream& operator>>(QDataStream& out_stream);
   friend QDataStream& operator<<(QDataStream& out_stream, MESSAGE_HEADER_ROTARY& Src);

};



//=====================================================================================
#include "arduino_json.h"

class CommandJson
{
  public:
  JsonDocument command;
   std::string command_string;
    QByteArray toByteArray() { return QByteArray(command_string.data(), command_string.size());};
};

class CommandSetPairJson : public CommandJson
{
  public:
  JsonDocument slave1;
  JsonDocument slave2;

  CommandSetPairJson();
  template<typename T> void setData(const QPair<T,T>& Pos);
  void setData(StateRotaryControl setting) ;

  template<typename T> void operator=(const QPair<T,T>& Pos)      { setData(Pos); };
                       void operator=(StateRotaryControl setting) { setData(setting); };

};

class CommandLidControlJson : public CommandJson
{
  public:
  JsonDocument command_settings;

  CommandLidControlJson(QString setting_str = "id: 1, type: leftCapOpen");
  void dumpToByteArray(QByteArray& output_array) { output_array = toByteArray(); };
  QByteArray castToByteArray() {QByteArray output_array = toByteArray(); return output_array; };
       void operator>>(QByteArray& output_array) { output_array = toByteArray(); };

  void printCommand() { qDebug() << "[ COMMAND LID ] " << command_string.c_str(); }
  template<typename T> void setData(T Param) {}; 

  static QString printDataType() { return "CommandLidControlJson"; }
};

class MessageRotaryStateJson : public CommandJson
{
  public:
  JsonDocument slave1;
  JsonDocument slave2;

  MessageRotaryStateJson();
  void setParams(QString setting1, 
                 QString setting2 = "state:OPERATIONAL, class:TDrive, name:Motor1, status:0, position:799246, velocity:0, torque:0, voltage:24590, analogIn:130, digitalIn:4128768");

    void loadData(QByteArray array)          { DeserializationError err = deserializeJson(command, array.data()); };
    void loadData(const char* data, int size)
    { 
      DeserializationError err = deserializeJson(command, data, size); 
      qDebug() << "MESSAGE LOAD DATA: " << QByteArray(data,size).toHex();
    };

    template<int SIZE>
    void loadData(std::array<uint8_t,SIZE> data)
    { 
      //qDebug() << "MESSAGE LOAD DATA ARRAY: " << QByteArray(data).toHex();
      DeserializationError err = deserializeJson(command, data); 
    };
  void operator>>(QByteArray& output_array) { output_array = toByteArray(); };

  template<int SIZE>
  void operator>>(std::array<uint8_t,SIZE>& output_array) 
  { 
    std::copy(command_string.begin(), command_string.end(), output_array.begin());
  };

        void printMessage() { serializeJson(command,command_string); qDebug() << "[ ROTARY STATE JSON ] " << command_string.c_str(); }
  static int getMaxSize();

};

struct MESSAGE_HEADER_ROTARY_JSON
{
  uint16_t  MESSAGE_IDENT = 0x7B22;  // идентификатор пакета
  uint16_t  RESERVE1 = 0x6964;        // резервное поле данных
  uint16_t  RESERVE2 = 0x223A;        // резервное поле данных
  uint16_t  RESERVE3 = 0x3132;        // резервное поле данных

                          bool isValid() { return MESSAGE_IDENT == 0x7B22 ; }
        static uint8_t getHeaderMarker() { MESSAGE_HEADER_ROTARY_JSON MESSAGE; return ((uint8_t*) &(MESSAGE.MESSAGE_IDENT))[0]; }
            static int getMaxSize()  { return MessageRotaryStateJson::getMaxSize(); }; 
  constexpr static int getMaxSizeStatic()  { return 500; }; 

  static constexpr int getHeaderSize() { return 8;};
                   int getMessageSize() { return getMaxSize();}; 
};

template<typename T> 
void CommandSetPairJson::setData(const QPair<T,T>& Pos)
{
  slave1["target"] = Pos.first; 
  slave2["target"] = Pos.second;

  command["slaves"][0] = slave1; command["slaves"][1] = slave2;

  serializeJson(command,command_string);
}
