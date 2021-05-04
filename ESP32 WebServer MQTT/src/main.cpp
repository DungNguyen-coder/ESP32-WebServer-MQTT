#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "myLib.h"

const int DHTPIN = 5;
const int DHTTYPE = DHT11;

DHT dht(DHTPIN, DHTTYPE);
StaticJsonDocument<200> json;
WebServer webServer(80);
const char *ssid = "P401";
const char *pass = "thaonghiadung";
//=========Biến chứa mã HTLM Website==//

const char *broker = "broker.hivemq.com";
WiFiClient tcpclient;
PubSubClient mqttclient(tcpclient);
const char *ID = "Dung-Nguyen-Deviot";
const char *topic[4] = {"ESP32/DungNguyen/DEVIOT/LED_D2", "ESP32/DungNguyen/DEVIOT/LED_D4", "ESP32/DungNguyen/DEVIOT/NHIETDO", "ESP32/DungNguyen/DEVIOT/DOAM"};

void reconnect()
{
  Serial.println("Lost Connection");
  while (!mqttclient.connected())
  {
    Serial.print("Retry Connection");
    if (mqttclient.connect(ID))
    {
      for (int i = 0; i < 4; i++)
      {
        mqttclient.subscribe(topic[i]);
      }

      Serial.println("connecting");
    }
    else
    {
      Serial.println("Try again !!!");
      delay(1000);
    }
  }
}

//=========================================//
void mainpage();
void on_D1();
void off_D1();
void on_D2();
void off_D2();
void get_statusD1D2();

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  WiFi.begin(ssid, pass);
  Serial.begin(115200);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("...");
  }
  Serial.println(WiFi.localIP());

  webServer.on("/", mainpage);
  webServer.on("/onD1", on_D1);
  webServer.on("/offD1", off_D1);
  webServer.on("/onD2", on_D2);
  webServer.on("/offD2", off_D2);
  webServer.on("/getstatusD1D2", get_statusD1D2);
  webServer.begin();
  dht.begin();

  mqttclient.setServer(broker, 1883);
  mqttclient.setCallback(callbackData);
  mqttclient.connect(ID);
  for (int i = 0; i < 4; i++)
  {
    mqttclient.subscribe(topic[i]);
  }
}

String LED1_Status = "ON";
String LED2_Status = "ON";
void loop()
{
  webServer.handleClient();
  if (!mqttclient.connected())
  {
    reconnect();
  }
  mqttclient.loop();
}
//==========Chương trình con=================//
void mainpage()
{
  String s = FPSTR(MainPage);
  webServer.send(200, "text/html", s);
}
void on_D1()
{
  digitalWrite(2, 1);
  LED1_Status = "ON";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D2", "21");
  webServer.send(200, "text/html", "CHÂN D1 ĐÃ ON");
}
void off_D1()
{
  digitalWrite(2, 0);
  LED1_Status = "OFF";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D2", "20");
  webServer.send(200, "text/html", "CHÂN D1 ĐÃ OFF");
}
void on_D2()
{
  digitalWrite(4, 1);
  LED2_Status = "ON";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D4", "41");
  webServer.send(200, "text/html", "CHÂN D2 ĐÃ ON");
}
void off_D2()
{
  digitalWrite(4, 0);
  LED2_Status = "OFF";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D4", "40");
  webServer.send(200, "text/html", "CHÂN D2 ĐÃ OFF");
}
void get_statusD1D2()
{
  //  String s = "{\"D1\": \""+ LED1_Status +"\",\"D2\": \""+ LED2_Status +"\",\"TEM\"}";
  json["D1"] = LED1_Status;
  json["D2"] = LED2_Status;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  json["NHIETDO"] = String(t);
  json["DOAM"] = String(h);

  String s = "";
  serializeJson(json, s);
  webServer.send(200, "application/json", s);
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/NHIETDO", String(t).c_str());
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/DOAM", String(h).c_str());
}

void callbackData(char *topic, byte *payLoad, unsigned int sizeData)
{
  String s = "";
  for (int i = 0; i < sizeData; i++)
  {
    s += (char)payLoad[i];
  }
  Serial.println(topic);
  Serial.println(s);
  Serial.println("----------------------------------");

  if (s[0] == '2')
  {
    if (s[1] == '1')
    {
      digitalWrite(2, 1);
      LED1_Status = "ON";
    }
    else
    {
      digitalWrite(2, 0);
      LED1_Status = "OFF";
    }
  }
  else if (s[0] == '4')
  {
    if (s[1] == '1')
    {
      digitalWrite(4, 1);
      LED2_Status = "ON";
    }
    else
    {
      digitalWrite(4, 0);
      LED2_Status = "OFF";
    }
  }
}