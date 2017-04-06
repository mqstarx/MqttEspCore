
#define WIFI_CONNECT_RETRIES 100

Bounce debouncer = Bounce();
//unsigned long t_Tek = 0; //текущий отсчет времени при определении нажатия







bool timer1_started = false;
bool timer2_started = false;
bool timer3_started = false;
bool timer4_started = false;
bool timer5_started = false;
bool timer6_started = false;
bool timerLed1_started = false;

unsigned long  timer1_tick_time = 15000;
unsigned long  timer2_tick_time = 30000;
unsigned long  timer3_tick_time = 180000;
unsigned long  timer4_tick_time = 180000;
unsigned long  timer5_tick_time = 10000;
unsigned long  timer6_tick_time = 30000;
unsigned long  timerLed1_tick_time = 500;

unsigned long  timer1_time_last;
unsigned long  timer2_time_last;
unsigned long  timer3_time_last;
unsigned long  timer4_time_last;
unsigned long  timer5_time_last;
unsigned long  timer6_time_last;
unsigned long  timerLed1_time_last;

void StartTimer1()
{
  timer1_started = true;
  timer1_time_last = millis();

}
void ResetTimer1()
{
  timer1_time_last = millis();
}
void StopTimer1()
{
  timer1_started = false;
}
void StartTimer2()
{
  timer2_started = true;
  timer2_time_last = millis();
}
void ResetTimer2()
{
  timer2_time_last = millis();
}
void StopTimer2()
{
  timer2_started = false;
}
void StartTimer3()
{
  timer3_started = true;
  timer3_time_last = millis();
}
void ResetTimer3()
{
  timer3_time_last = millis();
}
void StopTimer3()
{
  timer3_started = false;
}
void StartTimer4()
{
  timer4_started = true;
  timer4_time_last = millis();
}
void ResetTimer4()
{
  timer4_time_last = millis();
}
void StopTimer4()
{
  timer4_started = false;
}
void StartTimerLed1()
{
  timerLed1_started = true;
  timerLed1_time_last = millis();

}
void ResetTimerLed1()
{
  timerLed1_time_last = millis();
}
void StopTimerLed1()
{
  timerLed1_started = false;
}
void StartTimer5()
{
  timer5_started = true;
  timer5_time_last = millis();
}
void ResetTimer5()
{
  timer5_time_last = millis();
}
void StopTimer5()
{
  timer5_started = false;
}
void StartTimer6()
{
  timer6_started = true;
  timer6_time_last = millis();
}
void ResetTimer6()
{
  timer6_time_last = millis();
}
void StopTimer6()
{
  timer6_started = false;
}
/////////////////////////
void TimersProcced() // проверка состояния таймеров, для основного цикла loop
{
  if (millis() - timer1_time_last > timer1_tick_time && timer1_started )
  {
    TimerTick1();
  }
  if (millis() - timer2_time_last > timer2_tick_time && timer2_started)
  {
    TimerTick2();
  }
  if (millis() - timer3_time_last > timer3_tick_time && timer3_started)
  {
    TimerTick3();
  }
  if (millis() - timer4_time_last > timer4_tick_time && timer4_started)
  {
    TimerTick4();
  }
  if (millis() - timer5_time_last > timer5_tick_time && timer5_started)
  {
    TimerTick5();
  }
  if (millis() - timer6_time_last > timer6_tick_time && timer6_started)
  {
    TimerTick6();
  }
  if (millis() - timerLed1_time_last > timerLed1_tick_time && timerLed1_started )
  {
    TimerTickLed1();
  }
}
void InitRelays()
{

  pinMode(RELAY_OUTPUT, OUTPUT);
  delay(200);
  digitalWrite(RELAY_OUTPUT, false);

}
void InitSwithers()
{

  
  pinMode(SWITCHERS_OUTPUT, INPUT);
  delay(200);
  
   debouncer.attach(SWITCHERS_OUTPUT);
  debouncer.interval(100); // interval in ms
  debouncer.update();
  SWITCHERS_STATE = debouncer.read();
}
void ModuleStart() // начальный пуск модуля
{
  ESP.eraseConfig();
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Firmware version: "MOD_VERSION);
  pinMode(LEDPIN, OUTPUT);
  InitRelays();
  InitSwithers();
  EEPROM.begin(EEPROM_SIZE);
  // ResetEEPROM();
// WriteToEEPROM("ssid","GenDir");
 // WriteToEEPROM("ssid_pass","78906543");
  //WriteToEEPROM("mqtt_server","broker.mqttdashboard.com");
  // WriteToEEPROM("mqtt_port","1883");
  //WriteToEEPROM("mqtt_topic","MyDev1");
 // EEPROM.write(0, 0x25);
  //  EEPROM.commit();
  MODID = String(ESP.getChipId());
  MqttTopic = "SH_"+MODID;
  
  ConnectToWiFi();
  //CreateServer();
  
   
  //server.begin();
  //Udp.begin(666);
 // StartTimer6();

}

