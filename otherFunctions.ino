bool ConnectToWifiBasic(String Name, String Pass, int retries)
{
  WiFi.softAPdisconnect(true);
  Serial.println("Connecting to ");
  Serial.println(Name);
  Serial.println(Pass);
  WiFi.begin(Name.c_str(), Pass.c_str());
  int retries_count = 0;
  while ( WiFi.status() != WL_CONNECTED) {
    SW();
    delay(200);
    Serial.print(".");
    retries_count++;
    if (retries_count > retries)
      break;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    StartTimerLed1();
    return true;

  }
  else
  {
    Serial.println("");
    Serial.println("WiFi Not Connected");
    return false;
  }
}
void ConnectToWiFi()
{
  if (EEPROM.read(0) == 0x25)
  {
    Udp.stop();
    digitalWrite(LEDPIN, LOW);
    WiFi.mode(WIFI_STA);
    if (ConnectToWifiBasic(ReadEEPROM("ssid"), ReadEEPROM("ssid_pass"), WIFI_CONNECT_RETRIES))
    {
      StartTimer1();
      StopTimer4  ();
      MQTTConnect();
    }
    else
    {
      StartAP();
    }

  }
  else
  {
    StartAP();
  }
}
void StartAP()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  String res = "SH_" + MODID;
  WiFi.softAP(res.c_str(), "12345678");
  StopTimer1();
  StopTimer2();
  StopTimer3();
  StopTimer5();
  Serial.println("Fail. Ap Started");
  StartTimer4();
  StopTimerLed1();
  Udp.begin(666);
  digitalWrite(LEDPIN, LOW);
}
/*void CreateServer()
  {
  server.on("/getconfig", []()
  {
     server.send(200, "text/plain", "SSID:"+ReadEEPROM("ssid")+";PASS:"+ReadEEPROM("ssid_pass"));
  });
  server.begin();
  }*/
void SendUdp(String packet)
{

  Serial.println(packet);
  Udp.beginPacket(Udp.remoteIP(), 6666);
  Udp.write(packet.c_str());

  Udp.endPacket();

}
void SendUdpBroadCast(String packet)
{
  IPAddress broadcastIP(WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], 255);
  Serial.println(packet);
  Udp.beginPacket(broadcastIP, 6666);
  Udp.write(packet.c_str());

  Udp.endPacket();

}
void UdpBufferProcess(String packetBuffer )
{
  if (packetBuffer == "ASKID")
  {
    SendUdp("ID:SH_" + MODID + ";IP:" + GetIP());
  }
  if (packetBuffer.indexOf("CONFIG:") != -1)
  {

    WriteToEEPROM("ssid", SplitCommand(packetBuffer, ":", 2, 4));
    WriteToEEPROM("ssid_pass", SplitCommand(packetBuffer, ":", 3, 4));
    WriteToEEPROM("mqtt_devname", SplitCommand(packetBuffer, ":", 4, 4));
    EEPROM.write(0, 0x25);
    EEPROM.commit();
    SendUdp("WRITE_OK");
  }
   if (packetBuffer == "ASKCONFIG")
  {
    SendUdp("CONFIG:" +ReadEEPROM("ssid")+ ":" +ReadEEPROM("ssid_pass")+":"+ReadEEPROM("mqtt_devname"));
  }
  if (packetBuffer == "WIFICONNECT")
  {
    ConnectToWiFi();
  }

}
void UdpProcess()
{

  if (Udp.parsePacket())
  {
    //int packetSize = Udp.parsePacket();
    //  Serial.print("Received packet of size ");
    // Serial.println(packetSize);
    //   Serial.print("From ");
    //  IPAddress remoteIp = Udp.remoteIP();
    //  Serial.print(remoteIp);
    //  Serial.print(", port ");
    //  Serial.println(Udp.remotePort());
    // read the packet into packetBufffer
    int len = Udp.read(packetBuff, 255);
    if (len > 0) {
      packetBuff[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuff);
    UdpBufferProcess(packetBuff);
  }

}
void SW()
{
  debouncer.update();
  // Serial.println(String(debouncer.read()));
  if (debouncer.read() != SWITCHERS_STATE)
  {
    digitalWrite(RELAY_OUTPUT, !RELAY_STATE);
    RELAY_STATE = !RELAY_STATE;
    SWITCHERS_STATE = !SWITCHERS_STATE;
    if (client.connected())
      client.publish(MqttTopic + "/control/out", "STATE:" + String(RELAY_STATE));
    //need_pub_state = true;
  }
}
String GetIP()
{
  return String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
  //return String(WiFi.localIP()[0])+"."+String(WiFi.localIP()[1])+"."+"0"+"."+String(WiFi.localIP()[3]);
}
String SplitCommand ( String input_string, String separator, int segment_number, int arg_count)
{
  int from = 0;
  int h  = input_string.indexOf(separator, 0);
  for (int i = 1; i <= arg_count; i++)
  {

    if (i == segment_number) {

      if (segment_number == arg_count)
        return input_string.substring(from, input_string.length() );
      else
        return input_string.substring(from, h);
    }
    from = h + 1;
    h = input_string.indexOf(separator, h + 1);
  }
  return "";
}
/*void TCPServerProcessing()
  {
  if (!wclient_autoconf.connected())
  {
    wclient_autoconf = server.available();
    connected_to_smart = false;
  }
  else
  {
    // read data from the connected client
    connected_to_smart = true;
    if (wclient_autoconf.available() > 0)
    {
      String req = wclient_autoconf.readStringUntil('\r');
      if (req.length() > 0)
      {
        Serial.print(req);
        if (req == "ASKID")
        {
          wclient_autoconf.println("ID:" + MODID);
        }
        if (req.indexOf("SSID:") != -1)
        {

          WriteToEEPROM("ssid", SplitCommand(req, ":", 2, 2));
          EEPROM.write(0, 0x25);
          EEPROM.commit();
          wclient_autoconf.println("SSID WRITE OK");
          wclient_autoconf.println(SplitCommand(req, ":", 2, 2));
        }
        if (req.indexOf("SSID_PASS:") != -1)
        {
          WriteToEEPROM("ssid_pass", SplitCommand(req, ":", 2, 2));
          wclient_autoconf.println("SSID_PASS WRITE OK");
          wclient_autoconf.println(SplitCommand(req, ":", 2, 2));
        }
        if (req.indexOf("MQTT_SERVER:") != -1)
        {
          WriteToEEPROM("mqtt_server", SplitCommand(req, ":", 2, 2));
          wclient_autoconf.println("MQTT_SERVER WRITE OK");
          wclient_autoconf.println(SplitCommand(req, ":", 2, 2));
        }
        if (req.indexOf("MQTT_TOPIC:") != -1)
        {
          WriteToEEPROM("mqtt_topic", SplitCommand(req, ":", 2, 2));
          wclient_autoconf.println("MQTT_TOPIC WRITE OK");
          wclient_autoconf.println(SplitCommand(req, ":", 2, 2));
        }
        if (req.indexOf("MQTT_PORT:") != -1)
        {
          WriteToEEPROM("mqtt_port", SplitCommand(req, ":", 2, 2));
          wclient_autoconf.println("MQTT_PORT WRITE OK");
          wclient_autoconf.println(SplitCommand(req, ":", 2, 2));
        }
        if (req.indexOf("MQTT_DEVNAME:") != -1)
        {
          WriteToEEPROM("mqtt_devname", SplitCommand(req, ":", 2, 2));
          wclient_autoconf.println("MQTT_DEVNAME WRITE OK");
          wclient_autoconf.println(SplitCommand(req, ":", 2, 2));
        }
        if (req == "ASKSSID")
        {
          wclient_autoconf.println("SSID:" + ReadEEPROM("ssid") + "\r\n");
        }
        if (req == "ASKSSIDPASS")
        {
          wclient_autoconf.println("SSID_PASS:" + ReadEEPROM("ssid_pass") + "\r\n");
        }
        if (req == "ASKMQTTSERVER")
        {
          wclient_autoconf.println("MQTT_SERVER:" + ReadEEPROM("mqtt_server") + "\r\n");
        }
        if (req == "ASKMQTTTOPIC")
        {
          wclient_autoconf.println("MQTT_TOPIC:" + ReadEEPROM("mqtt_topic") + "\r\n");
        }
        if (req == "ASKMQTTPORT")
        {
          wclient_autoconf.println("MQTT_PORT:" + ReadEEPROM("mqtt_port") + "\r\n");
        }
        if (req == "ASKMQTTDEVNAME")
        {
          wclient_autoconf.println("MQTT_DEVNAME:" + ReadEEPROM("mqtt_devname") + "\r\n");
        }
        if (req == "MQTTCONNECT")
        {
          wclient_autoconf.println("CONNECTING_MQTT...");
          MQTTConnect();
        }
        if (req == "WIFICONNECT")
        {
          wclient_autoconf.println("CONNECTING_TO_WIFI...");

          if (EEPROM.read(0) == 0x25)
          {
            digitalWrite(LEDPIN,LOW);
            WiFi.mode(WIFI_STA);
            if (ConnectToWifi(ReadEEPROM("ssid"), ReadEEPROM("ssid_pass"), WIFI_CONNECT_RETRIES))
            {
              StartTimer1();
              //  server.stop();
              // Udp.begin(666);
              MQTTConnect();
            }
            else
            {
              WiFi.disconnect();
              WiFi.mode(WIFI_AP);
              String res = "SH_" + MODID;
              WiFi.softAP(res.c_str(), "12345678");
              StartTimer4();
            }

          }
          else
          {
            WiFi.mode(WIFI_AP);
            String res = "SH_" + MODID;
            WiFi.softAP(res.c_str(), "12345678");
          }
        }
        if (req == "MQTTCLOSE")
        {
          wclient_autoconf.println("DISCONECTING...");
          MQTTClose();
        }
        if (req == "RESET_EEPROM")
        {
          wclient_autoconf.println("RESETING_EEPROM...");
          ResetEEPROM();

        }
        if (req == "RESTART")
        {
          wclient_autoconf.println("RESTARTING...");
          ESP.reset();


        }

        if (req == "ASKVERSION")
        {
          wclient_autoconf.println("FIRMWARE:" + String(MOD_VERSION));
          // UpdateBin();

        }
        if (req == "UPDATE")
        {
          wclient_autoconf.println("UPDATING_FLASH...");
          UpdateBin();

        }
      }
    }
  }
  }*/
void UpdateBin()
{
  t_httpUpdate_return ret = ESPhttpUpdate.update("http://smarthomemod.ucoz.net/CoreMqttV2.ino.nodemcu.bin");
  //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }

}
