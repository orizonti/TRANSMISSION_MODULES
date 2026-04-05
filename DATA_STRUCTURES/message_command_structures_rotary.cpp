#include "message_command_structures_rotary.h"

  void SystemState::operator=(SystemState& new_state)
  {
  version = new_state.version; // версия протокола
    state = new_state.state;   // байт состояния системы
   drives = new_state.drives;  // количество приводов
     temp = new_state.temp;    // температура вычислителя в градусах Цельсия
  temp1_0 = new_state.temp1_0; // температура приводов в градусах Цельсия
  temp1_1 = new_state.temp1_1; // температура приводов в градусах Цельсия
  temp2_0 = new_state.temp2_0; // температура оптических клиньев в градусах Цельсия
  temp2_1 = new_state.temp2_1; // температура оптических клиньев в градусах Цельсия
  }

int MESSAGE_HEADER_ROTARY::getMessageSize() 
{  
  if( MESSAGE_IDENT == 0xA001 ) return sizeof(SystemState) + sizeof(MESSAGE_HEADER_ROTARY); 
  if( MESSAGE_IDENT == 0xA002 ) return sizeof(ControlRX)   + sizeof(MESSAGE_HEADER_ROTARY); return 0; 
}

QDataStream& operator<<(QDataStream& out_stream, MESSAGE_HEADER_ROTARY& Src)
{
    out_stream << Src.TIME.sec << Src.TIME.nsec << Src.MESSAGE_IDENT << Src.RESERVE;
    return out_stream;
}

QDataStream& MESSAGE_HEADER_ROTARY::operator>>(QDataStream& out_stream) 
{ 
  out_stream << TIME.sec << TIME.nsec << MESSAGE_IDENT << RESERVE; return out_stream;
  return out_stream;
};

//=========================================================
//ROTARY CONTROL GENERIC STATE 
void ControlTX::operator=(StateRotaryControl setting) 
{
  value0.target = setting.Engine1.Position; 
  value1.target = setting.Engine2.Position;

  setMode(setting.Engine1.Mode);
  setAccelLimit(setting.Engine1.Limits[2]);
};

void ControlTX::setData(StateRotaryControl setting) 
{
  value0.target = setting.Engine1.Position; 
  value1.target = setting.Engine2.Position;

  setMode(setting.Engine1.Mode);
  setAccelLimit(setting.Engine1.Limits[2]);
};

void operator>>(QDataStream& stream, ControlTX& rec) { stream >> rec.value0 >> rec.value1; }
void ControlTX::operator>>(QDataStream& stream) 
{ 
//  stream << value0.mode << value0.digital << value0.reserve << value0.acc_limit << value0.target;           
//  stream << value1.mode << value1.digital << value1.reserve << value1.acc_limit << value1.target;           

  stream << value0.mode << value0.Param2 << value0.Param3 << value0.Param4 
         << value0.Param5 << value0.Param6 << value0.Param7 << value0.Param8 << value0.target;           
  stream << value1.mode << value1.Param2 << value1.Param3 << value1.Param4 
         << value1.Param5 << value1.Param6 << value1.Param7 << value1.Param8 << value1.target;           
}
void operator<<(QDataStream& stream, ControlTX& src) 
{ 
//  stream << src.value0.mode << src.value0.digital << src.value0.reserve << src.value0.acc_limit << src.value0.target; 
//  stream << src.value1.mode << src.value1.digital << src.value1.reserve << src.value1.acc_limit << src.value1.target; 

  stream << src.value0.mode << src.value0.Param2 << src.value0.Param3 << src.value0.Param4 
         << src.value0.Param5 << src.value0.Param6 << src.value0.Param7 << src.value0.Param8 << src.value0.target;           
  stream << src.value1.mode << src.value1.Param2 << src.value1.Param3 << src.value1.Param4 
         << src.value1.Param5 << src.value1.Param6 << src.value1.Param7 << src.value1.Param8 << src.value1.target;           
}

//=========================================================
//ROTARY CONTROL JSON

CommandSetPairJson::CommandSetPairJson()
{
  command["id"] = 123;
  command["dtg"] = 1111;

  slave1["name"] = "joint1"; slave1["mode"] = "position"; slave1["class"] = "TDrive"; slave1["target"] = 0;
  slave2["name"] = "joint2"; slave2["mode"] = "position"; slave2["class"] = "TDrive"; slave2["target"] = 0;

  command["slaves"][0] = slave1;
  command["slaves"][1] = slave2;

  size_t size = serializeJson(command,command_string); 
}

void CommandSetPairJson::setData(StateRotaryControl setting) 
{
  slave1["target"] = setting.Engine1.Position; 
  slave2["target"] = setting.Engine2.Position;

  if(setting.Engine1.Mode == 0) slave1["mode"] = "position"; if(setting.Engine1.Mode == 1) slave1["mode"] = "velocity";
  if(setting.Engine1.Mode == 0) slave2["mode"] = "position"; if(setting.Engine1.Mode == 1) slave1["mode"] = "velocity";
};


QDataStream& operator>>(QDataStream& stream, DriveTX& rec)
{
  //stream >> rec.mode >> rec.digital >> rec.reserve >> rec.acc_limit >> rec.target; 
  //stream >> rec.mode >> rec.digital >> rec.reserve >> rec.acc_limit >> rec.target; 
  return stream;
}

MessageRotaryStateJson::MessageRotaryStateJson() 
{
command["id"] = 123;
command["dtg"] = 1111;

slave1["state"] = "OPERATIONAL"; 
slave1["class"] = "TDrive"; 
slave1["name"] = "Motor1"; 
slave1["status"] = 0;
slave1["position"] = 0;
slave1["velocity"] = 0;
slave1["torque"] = 0;
slave1["voltage"] = 0;
slave1["analogIn"] = 0;
slave1["digitalIn"] = 0;

slave2 = slave1;
slave1["name"] = "Motor2"; 

command["slaves"][0] = slave1;
command["slaves"][1] = slave2;

size_t size = serializeJson(command,command_string); 
}

void MessageRotaryStateJson::setParams(QString setting1, QString setting2)
{
    QStringList settings_list = setting1.split(",");
for(auto setting_str: settings_list)
{
              auto setting_pair = setting_str.split(":");
  slave1[setting_pair[0].trimmed().toStdString()] = setting_pair[1].trimmed().toStdString(); 
}

                settings_list = setting2.split(",");
for(auto setting_str: settings_list)
{
              auto setting_pair = setting_str.split(":");
  slave2[setting_pair[0].trimmed().toStdString()] = setting_pair[1].trimmed().toStdString(); 
}

command["slaves"][0] = slave1;
command["slaves"][1] = slave2;
size_t size = serializeJson(command,command_string); 
}

int MessageRotaryStateJson::getMaxSize()
{
  MessageRotaryStateJson message;
  QString str1("state:OPERATIONAL, class:TDrive, name:Motor1, status:0, position:10000, velocity:10000, torque:1000, voltage:24590, analogIn:1300, digitalIn:4128768");
  QString str2("state:OPERATIONAL, class:TDrive, name:Motor2, status:0, position:10000, velocity:10000, torque:1000, voltage:24590, analogIn:1300, digitalIn:4128768");

         message.setParams(str1, str2);
  return message.toByteArray().size() + 50;

}
//=========================================================
//LID CONTROL JSON
CommandLidControlJson::CommandLidControlJson(QString setting_str )
{
      QStringList settings_list = setting_str.split(",");
  for(auto setting_str: settings_list)
  {
                auto setting_pair = setting_str.split(":");
  //  command_settings[setting_pair[0].trimmed().toStdString()] = setting_pair[1].trimmed().toStdString(); 
      command[setting_pair[0].trimmed().toStdString()] = setting_pair[1].trimmed().toStdString();
  }
  size_t size = serializeJson(command,command_string); 
}
//=========================================================