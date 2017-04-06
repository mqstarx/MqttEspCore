void TimerTick1()
{
  Serial.println("WifiCheckConnection...");
  if (WiFi.status() != WL_CONNECTED)
  {
    StartAP();
  }
  else
  {
    Serial.println( ESP.getFreeHeap());
     
    Serial.println("WifiOK");
  //  Udp.stop();
   // int u = Udp.begin(666);
   // Serial.println(String(u));
    ResetTimer1();
  }
}
void TimerTick2()
{
  Serial.println("TickTimer2");
  if (!client.connected())
  {
     StartTimer3();
     StartTimerLed1();
     StopTimer2();
     StopTimer5();
  }
  else
  {
    Serial.println("MQTT_OK");
     ResetTimer2();
  }
}
void TimerTick3()
{
   Serial.println("TickTimer3");
    MQTTConnect();
    ResetTimer3();
}
void TimerTick4()
{
 // Serial.println(String(WiFi.softAPgetStationNum()));
  Serial.println("TickTimer4");
  if (WiFi.status() != WL_CONNECTED  && WiFi.softAPgetStationNum()==0)
  {
   ConnectToWiFi();
  }

  ResetTimer4();
}
void TimerTick5()
{
   SendUdpBroadCast("ID:SH_"+MODID+";NAME:"+ReadEEPROM("mqtt_devname"));
  // Udp.stop();
   //Udp.begin(666);
    ResetTimer5();
}
void TimerTick6()
{
   Serial.println("TickTimer6 UD server Restarting");
   //Udp.stop();
  // Udp.begin(666);
    ResetTimer6();
}
void TimerTickLed1()
{
   Serial.println("TickTimerLed1");
  digitalWrite(LEDPIN, !ledState);
  ledState=!ledState;
  ResetTimerLed1();
}

