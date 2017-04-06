#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Bounce2.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
//#include <ESP8266WebServer.h>

bool RELAY_STATE = false; //  состояний реле
int RELAY_OUTPUT =  5; // инициализация пинов на релюхи
int SWITCHERS_OUTPUT = 4; // инициализация пинов выключателей
bool SWITCHERS_STATE = false; // состояний выключателей
bool connected_to_smart=false;
String MODID;
unsigned long millis_prev;
bool ledState =false;
#define MOD_VERSION "0.0.0.16"
WiFiClient wclient;
//WiFiServer server(48655);
//WiFiClient wclient_autoconf;
WiFiUDP Udp;
//ESP8266WebServer server(8080);
PubSubClient client(wclient);
char packetBuff[255];
#define LEDPIN 2
String MqttTopic;

void MQTTClose()
{
  Serial.println("MQTT closing conection");
  client.disconnect();
  if(!client.connected())
  {
     StartTimerLed1();
     StopTimer2();
  }
  
}
void MQTTConnect()
{
  
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!client.connected())
    {
    //  Serial.println(ReadEEPROM("mqtt_server") + "---" + ReadEEPROM("mqtt_port"));
     // client.set_server(ReadEEPROM("mqtt_server"), ReadEEPROM("mqtt_port").toInt());
     // Serial.println("Connecting to MQTT server ...");
     client.set_server("test.mosca.io", 1883);
      bool success;
      if (EEPROM.read(1025) == 0x25 )
      {
        success = client.connect( MQTT::Connect( "SH_"+MODID ).set_auth(ReadEEPROM("mqtt_user"), ReadEEPROM("mqtt_pass")) );
      }
      else
      {
        success = client.connect( "SH_"+MODID );
      }
      if (success)
      {
        Serial.println("Connect to MQTT server: Success");
        //client.subscribe(MqttTopic);                // for receiving HELLO messages
        client.subscribe(MqttTopic + "/control/" ); // for receiving GPIO messages
        delay(200);
        Serial.println("Subscribe: Success");
        client.set_callback(callback);
        StartTimer2();
        StopTimer3();
        StopTimerLed1();
        StartTimer5();
         digitalWrite(LEDPIN,HIGH);
        
      }
      else
      {
        Serial.println("Connect to MQTT server: FAIL");
        StartTimer3();
        StartTimerLed1();
        digitalWrite(LEDPIN,LOW);
      }

    }
  }
}
void callback(const MQTT::Publish & sub)
{


  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());
  Serial.print(" => ");
  Serial.println(sub.payload_string());

  if (sub.topic() == MqttTopic + "/control/")
  {
    if (sub.payload_string() == "OFF")
    {
      RELAY_STATE = false;
      digitalWrite(RELAY_OUTPUT, RELAY_STATE);
      client.publish(MqttTopic + "/control/out", "STATE:" + String(RELAY_STATE));

    }
    if (sub.payload_string() == "ON")
    {

      RELAY_STATE = true;
      digitalWrite(RELAY_OUTPUT, RELAY_STATE);
      client.publish(MqttTopic + "/control/out", "STATE:" + String(RELAY_STATE));
    }
     if (sub.payload_string() == "RESET")
    {

    // ResetEEPROM();
     client.publish(MqttTopic + "/control/out", " Starting AP");
     StartAP();
     
    }
    if (sub.payload_string() == "ASKSTATE")
    {
      client.publish(MqttTopic + "/control/out", "STATE:" + String(RELAY_STATE));
    }
    if (sub.payload_string() == "ASKIP")
    {
      client.publish(MqttTopic+ "/control/out", "IP:" + GetIP());
    }
    if (sub.payload_string() == "UPDATE")
    {
      UpdateBin();
      client.publish(MqttTopic+ "/control/out", "UPDATE TRY");
    }
  }
  /*else if (sub.topic() == ReadEEPROM("mqtt_topic"))
  {
    if (sub.payload_string() == "HELLO")
    {
      client.publish(ReadEEPROM("mqtt_topic"), "ID:" + MODID + ";STATE:" + String(RELAY_STATE)+";IP:"+GetIP());

    }
  }*/

  
}
void setup()
{

  millis_prev = millis();
  ModuleStart();
  

}

void loop()
{

  UdpProcess();
  TimersProcced();
  //TCPServerProcessing();
  SW();

//server.handleClient();
  if (client.connected())
  {
    client.loop();
  }


}
