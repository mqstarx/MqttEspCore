#define EEPROM_SIZE 8192

//чтение памяти
String ReadFromEEPROM(int addr_from, int addr_to)
{
  String Result ;
  for (int i = addr_from; i <= addr_to; i++)
  {
    if (EEPROM.read(i) == 0)
      break;
    Result += (char)EEPROM.read(i);
  }
  return Result;
}

String ReadEEPROM(String param_name)
{
  if (param_name == "ssid")
  {
    return ReadFromEEPROMAddr(1);
  }
  if (param_name == "ssid_pass")
  {
    return ReadFromEEPROMAddr(257);
  }
  if (param_name == "mqtt_server")
  {
    return ReadFromEEPROMAddr(513);
  }
  if (param_name == "mqtt_port")
  {
    return ReadFromEEPROMAddr(769);
  }
  if (param_name == "mqtt_user")
  {
    return ReadFromEEPROMAddr(1026);
  }
  if (param_name == "mqtt_pass")
  {
    return ReadFromEEPROMAddr(1282);
  }
  if (param_name == "mqtt_pass")
  {
    return ReadFromEEPROMAddr(1282);
  }
  if (param_name == "mqtt_topic")
  {
    return ReadFromEEPROMAddr(1538);
  }
  if (param_name == "mqtt_devname")
  {
    return ReadFromEEPROMAddr(1794);
  }
  if (param_name == "mqtt_cat")
  {
    return ReadFromEEPROMAddr(2050);
  }
}
String ReadFromEEPROMAddr(int addr_from)
{
  String Result ;
  int len = EEPROM.read(addr_from);
  for (int i = 0; i < len; i++)
  {
    if (i > 220)
      break;

    Result += (char)EEPROM.read(i + addr_from + 1);
  }
  return Result;
}
//запись в память
void WriteToEEPROM(String param_name, String Data)
{
  if (param_name == "ssid")
  {
    WriteToEEPROMLen(Data, 1);
  }
  if (param_name == "ssid_pass")
  {
    WriteToEEPROMLen(Data, 257);
  }
  if (param_name == "mqtt_server")
  {
    WriteToEEPROMLen(Data, 513);
  }
  if (param_name == "mqtt_port")
  {
    WriteToEEPROMLen(Data, 769);
  }
  if (param_name == "mqtt_user")
  {
    WriteToEEPROMLen(Data, 1026);
  }
  if (param_name == "mqtt_pass")
  {
    WriteToEEPROMLen(Data, 1282);
  }
  if (param_name == "mqtt_pass")
  {
    WriteToEEPROMLen(Data, 1282);
  }
  if (param_name == "mqtt_topic")
  {
    //Serial.println("topic_write");
    WriteToEEPROMLen(Data, 1538);
  }
  if (param_name == "mqtt_devname")
  {
    WriteToEEPROMLen(Data, 1794);
  }
  if (param_name == "mqtt_cat")
  {
    WriteToEEPROMLen(Data, 2050);
  }
}
void WriteToEEPROMLen(String Data, int addr_from)
{
  EEPROM.write(addr_from, Data.length());
  for (int i = 0; i < Data.length(); i++)
  {
    if (i > 220)
      break;
    EEPROM.write(i + addr_from + 1, Data[i]);
  }
  EEPROM.commit();
}
void ResetEEPROM()
{
  for (int i = 0; i < EEPROM_SIZE; i++)
  {

    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}
