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


template<int NUM_DEV>
struct CommandDeviceRedux
{
  public:
  uint8_t Command = 0;
  uint8_t Param = 0;
  uint8_t Reserv1 = 0;
  uint8_t Reserv2 = 0;
  uint8_t Reserv3 = 0;
  uint8_t Reserv4 = 0;
  uint8_t Reserv5 = 0;
  uint8_t Reserv6 = 0;

  friend void operator<<(QDataStream& stream, CommandDeviceRedux& command)
  {
    stream << command.Command;
    stream << command.Param;
    stream << command.Reserv1 << command.Reserv2 << command.Reserv3 << command.Reserv4 << command.Reserv5 << command.Reserv6;
  };

  std::string print() { std::stringstream stream; stream << Command << " "<< Param; return stream.str(); }
};

template<int NUM_DEV>
struct RequestDeviceRedux
{
  public:
  uint8_t Command = 0;
  uint8_t Param = 0;
  uint8_t Reserv1 = 0;
  uint8_t Reserv2 = 0;
  uint8_t Reserv3 = 0;
  uint8_t Reserv4 = 0;
  uint8_t Reserv5 = 0;
  uint8_t Reserv6 = 0;

  std::string print() { std::stringstream stream; stream << Command << " "<< Param; return stream.str(); }
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

  std::string print() { std::stringstream stream; stream << ModuleID << " " << Command; return stream.str(); }
};


template<int N_CHAN>
class CommandSetPair
{
   public:
   float Param1 = 0;
   float Param2 = 0;

   public:
   template<typename T> void operator=(const QPair<T,T>& Pos)      {Param1 = Pos.first;                Param2 = Pos.second; };
                        void operator=(StateRotaryControl setting) {Param1 = setting.Engine1.Position; Param2 = setting.Engine2.Position; };

   template<typename T> void setData(const QPair<T,T>& Pos)      {Param1 = Pos.first; Param2 = Pos.second; };
                      void   setData(StateRotaryControl setting) {Param1 = setting.Engine1.Position; Param2 = setting.Engine2.Position; }

   friend void operator<<(QDataStream& stream, CommandSetPair& command) { stream << command.Param1 << command.Param2; };

   public:

   std::string print() { std::stringstream stream; stream << Param1 << " " << Param1; return stream.str(); }
};

template<int N_CHAN>
class CommandAiming
{
   public:
   CommandAiming(){};
   CommandAiming(float x, float y, float x_corr, float y_corr): AimPos{x,y}, AimCorrection{x_corr, y_corr} {};
   QPair<float,float> AimPos;
   QPair<float,float> AimCorrection;

   public:
   template<typename T> void operator=(const QPair<T,T>& Pos) { AimPos = Pos; };
   template<typename T> void setData  (const QPair<T,T>& Pos) { AimPos = Pos; };
   template<typename T> void setData  (const QPair<T,T>& Pos, const QPair<T,T>& Correction) 
   { 
    AimPos = Pos; AimCorrection = Correction; };

   friend void operator<<(QDataStream& stream, CommandAiming& command) { stream << command.AimPos.first 
                                                                                << command.AimPos.second 
                                                                                << command.AimCorrection.first 
                                                                                << command.AimCorrection.second; };


   std::string print() { std::string str; std::stringstream stream; stream << AimPos.first  << " " << AimPos.second << AimCorrection.first << " " << AimCorrection.second ;  return stream.str(); }
    
};

template<int NUM_DEV>
struct RequestDevice
{
    uint8_t DeviceID = 0;
    uint8_t Module = 0;
   uint16_t Param1 = 0;
   uint16_t Param2 = 0;
   uint16_t Param3 = 0;


   std::string print() { std::stringstream stream; stream << DeviceID << " "
                                                          << Module   << " " 
                                                          << Param1   << " " 
                                                          << Param2   << " " 
                                                          << Param3 ; return stream.str(); }
};

template<int N_CHAN>
struct RequestMoveState
{
   public:
   float Position1 = 0;
   float Position2 = 0;
   float Velocity1 = 1;
   float Velocity2 = 1;

 template<int NUM> friend void operator<<(StateRotaryControl& Receiver, RequestMoveState<NUM>& state);


   std::string print() { std::stringstream stream; stream << Position1 << " "
                                                          << Position1 << " " 
                                                          << Velocity1 << " " 
                                                          << Velocity2 ;  return stream.str(); }
};

template<int N_CHAN>
struct RequestPositionState
{
    public:
    uint16_t Position1;
    uint16_t Position2;
    uint16_t Sensor1;
    uint16_t Sensor2;

   std::string print() { std::stringstream stream; stream << Position1 << " "
                                                          << Position1 << " " 
                                                          << Sensor1   << " " 
                                                          << Sensor2 ;  return stream.str(); }
};

template<int N_CHAN>
struct RequestAimingState
{
    public:
    uint16_t Position1;
    uint16_t Position2;

    uint16_t Velocity;
    uint16_t Acceleration;

    uint16_t Dispersion;
    uint16_t TraitAim;

    float ErrorAiming;
    float TrackParam1;
    float TrackParam2;
    float TrackParam3;
};

struct CommandCheckConnection  { uint8_t Connect  = 0xC5; uint8_t Connect2 = 0xC6; };
struct CommandCloseConnection  { uint8_t Close1   = 0xC6; uint8_t Close2   = 0xC6; };

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
       using RequestPosRotary   = RequestMoveState<0>;
       using RequestPosScanator = RequestMoveState<1>;

using CommandAiming1 = CommandAiming<0>;
using CommandAiming2 = CommandAiming<1>;
       using RequestAiming = RequestAimingState<1>;
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

template<int NUM>
void operator<<(StateRotaryControl& Receiver, RequestMoveState<NUM>& state)
{
  Receiver.Engine1.Position = state.Position1;
  Receiver.Engine2.Position = state.Position2;
  Receiver.Engine1.Velocity = state.Velocity1;
  Receiver.Engine2.Velocity = state.Velocity2;
};