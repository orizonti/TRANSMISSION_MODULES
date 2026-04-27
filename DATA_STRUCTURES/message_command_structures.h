#pragma once
#include <stdint.h>
#include <string>
#include <sstream>
#include <QDataStream>
#include <QIODevice>
#include <QByteArray>
#include <QDebug>
#include "message_command_structures_rotary.h"
#include "message_command_structures_intermediate.h"


template<typename T> concept HAS_COMMAND_FIELD = requires(T t) { t.Command; };
template<typename T> concept HAS_PARAM_FIELD = requires(T t) { t.Param1; };
template<class COMMAND_TYPE>
concept COMMAND_STANDART = requires(COMMAND_TYPE M) 
{ 
    requires HAS_COMMAND_FIELD<COMMAND_TYPE> || HAS_PARAM_FIELD<COMMAND_TYPE>;
};

template<int NUM_DEV>
struct CommandDeviceRedux
{
  public:
  uint8_t Command = 0;
  uint8_t Param1 = 0;
  uint8_t Reserv1 = 0;
  uint8_t Reserv2 = 0;
  uint8_t Reserv3 = 0;
  uint8_t Reserv4 = 0;
  uint8_t Reserv5 = 0;
  uint8_t Reserv6 = 0;

  friend void operator<<(QDataStream& stream, CommandDeviceRedux& command)
  {
    stream << command.Command;
    stream << command.Param1;
    stream << command.Reserv1 << command.Reserv2 << command.Reserv3 << command.Reserv4 << command.Reserv5 << command.Reserv6;
  };

  std::string print() { std::stringstream stream; stream << Command << " "<< Param1; return stream.str(); }
};

template<int NUM_DEV>
struct RequestDeviceRedux
{
  public:
  uint8_t Command = 0;
  uint8_t Param1 = 0;
  uint8_t Reserv1 = 0;
  uint8_t Reserv2 = 0;
  uint8_t Reserv3 = 0;
  uint8_t Reserv4 = 0;
  uint8_t Reserv5 = 0;
  uint8_t Reserv6 = 0;

  std::string print() { std::stringstream stream; stream << Command << " "<< Param1; return stream.str(); }
};

template<int NUM_DEV>
struct CommandDevice
{
  public:
  uint16_t ModuleID = 0;
  uint16_t Command = 0;

  union Param
  {
  uint32_t ParamInt = 0;
     float ParamFloat;
  };

   void operator=(const CommandDevice<NUM_DEV>& Copy) { ModuleID = Copy.ModuleID; Command = Copy.Command; };

   friend void operator<<(QDataStream& stream, CommandDevice<NUM_DEV>& command) 
   { stream << command.ModuleID << command.Command << command.ParamInt; };

  std::string print() { std::stringstream stream; stream << ModuleID << " " << Command; return stream.str(); }
};


template<int N_CHAN>
class CommandSetPair
{
   public:
   std::pair<float,float> Command;

   CommandSetPair() { Command.first = 0; Command.second = 0; }
   CommandSetPair(float value, float value2) { Command.first = value; Command.second = value2; }

   public:
   void operator=(const CommandSetPair<N_CHAN>& Copy) { Command = Copy.Command; };
   template<typename T> void operator=(const QPair<T,T>& Pos)      { Command = Pos; };
                        void operator=(StateRotaryControl setting) { Command.first  = setting.Engine1.Position; 
                                                                     Command.second = setting.Engine2.Position; };

   template<typename T> void setData(const QPair<T,T>& Pos)      { Command = Pos; };
                      void   setData(StateRotaryControl setting) { Command.first = setting.Engine1.Position; 
                                                                   Command.second = setting.Engine2.Position; }

                                                
   friend void operator<<(QDataStream& stream, CommandSetPair<N_CHAN>& command) 
   { 
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << command.Command.first << command.Command.second; 
   };

   void dumpToByteArray(QByteArray& RawData) 
   { 
    QDataStream stream(&RawData, QIODevice::ReadWrite);
                stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << Command.first << Command.second; 
   };

   
   public:

   std::string print() { std::stringstream stream; stream << Command.first << " " << Command.second; return stream.str(); }
};

template<int N_CHAN>
class CommandAiming
{
   public:
                 uint32_t CommandType = 0;
   std::pair<float,float> Command;

   CommandAiming(){};
   CommandAiming(float x, float y): Command{x,y} {};
   CommandAiming(std::pair<float,float> pair): Command{pair} {};

   public:
                        void operator=(const CommandAiming<N_CHAN>& Copy) { Command = Copy.Command; CommandType = Copy.CommandType; };
   template<typename T> void operator=(const QPair<T,T>& Pair) { Command = Pair; };
   template<typename T> void setData  (const QPair<T,T>& Pair) { Command = Pair; };
   template<typename T> void setData  (const QPair<T,T>& Pair, uint32_t type) { Command = Pair; CommandType = type; };

   std::string print() { std::string str; std::stringstream stream; stream << Command.first<<" "<< Command.second; return stream.str(); }

   friend void operator<<(QDataStream& stream, CommandAiming<N_CHAN>& copy) 
   { 
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << copy.CommandType << copy.Command.first << copy.Command.second; 
   };
    
};

template<int NUM_DEV>
struct RequestDevice
{
    uint8_t DeviceID = 0;
    uint8_t Module = 0;
   uint16_t Param1 = 0;
   uint16_t Param2 = 0;
   uint16_t Param3 = 0;


   void operator=(const RequestDevice<NUM_DEV>& Copy) 
   { DeviceID = Copy.ID; Module = Copy.Module; Param1 = Copy.Param1; Param2 = Copy.Param2; Param3 = Copy.Param3; };

   std::string print() { std::stringstream stream; stream << DeviceID << " "
                                                          << Module   << " " 
                                                          << Param1   << " " 
                                                          << Param2   << " " 
                                                          << Param3 ; return stream.str(); }
};

template<int N_CHAN, typename T = float>
struct RequestState
{
   public:
   T Param1 = 0;
   T Param2 = 0;
   T Param3 = 1;
   T Param4 = 1;

   void operator=(const RequestState<N_CHAN,T>& Copy) 
   { Param1 = Copy.Param1; Param3 = Copy.Param3; 
     Param2 = Copy.Param2; Param4 = Copy.Param4; };

   std::string print() { std::stringstream stream; stream << Param1 << " "
                                                          << Param2 << " " 
                                                          << Param3 << " " 
                                                          << Param4 ;  return stream.str(); }

friend void operator<<(StateRotaryControl& Receiver, RequestState<N_CHAN,T>& state)
{
  Receiver.Engine1.Position = state.Param1;
  Receiver.Engine2.Position = state.Param2;
  Receiver.Engine1.Velocity = state.Param3;
  Receiver.Engine2.Velocity = state.Param4;
};
};


template<int N_CHAN, typename T = float>
struct RequestStateExtended
{
    public:
    T Param1; 
    T Param2; 
    T Param3; 
    T Param4; 
    T Param5; 
    T Param6; 

    uint32_t State1; 
    uint32_t State2; 
    uint32_t State3; 
    uint32_t State4; 
    uint32_t State5; 
    uint32_t State6; 
};

struct CommandCloseConnection  { uint8_t Comman = 0xC6; uint8_t Param = 0; };
struct CommandCheckConnection  
{ 
  public:
  uint8_t Command  = 0xC5; uint8_t Param = 0; 
  bool operator==(CommandCheckConnection copy) { return (Command == copy.Command && Param == copy.Param); }
};

struct CommandCalibration
{
    public:
    uint16_t  NodeType    = 0xA0;
    uint16_t  Command     = 0xA1;
    uint16_t  Channel     = 1;
    uint16_t  Amplitude   = 1;

    uint16_t  PeriodProcess = 1;
    uint16_t  TimeMeasure   = 1;
    uint16_t  NumberSteps   = 1;
    uint16_t  Reserve3      = 1;
};

using CommandSetPosRotary   = CommandSetPair<0>;
using CommandSetPosScanator = CommandSetPair<1>;
       using RequestPosRotary   = RequestState<0,float>;
       using RequestPosScanator = RequestState<1,uint16_t>;

using CommandAiming1 = CommandAiming<0>;
using CommandAiming2 = CommandAiming<1>;
       using RequestAiming = RequestStateExtended<1,float>;
//=====================================================
using CommandDeviceLaserPointer = CommandDeviceRedux<0>;
using CommandDeviceLaserPower   = CommandDeviceRedux<1>;
using CommandDeviceFocusator    = CommandDeviceRedux<2>;

using RequestDeviceController   = RequestDevice<0>;
using RequestDeviceLaserPower   = RequestDevice<1>;
using RequestDeviceLaserPointer = RequestDevice<2>;
using RequestDeviceFocusator    = RequestDevice<3>;
//=====================================================


#define LASER_CHECK 0x20
#define LASER_ON    0x22
#define LASER_OFF   0x26

#define LASER_SET_POWER 0x23
#define LASER_BEAM_ON   0x24
#define LASER_BEAM_OFF  0x25
#define LASER_PILOT_ON  0x27
#define LASER_PILOT_OFF 0x28

#define LASER_FAULT 0x39

#define LASER_MODULE       0 
#define LASER_MODULE_BEAM  1
#define LASER_MODULE_PILOT 2
#define LASER_MODULE_POWER 3

template<typename T> bool isAligned() { return sizeof(T) == T::getSize(); }
