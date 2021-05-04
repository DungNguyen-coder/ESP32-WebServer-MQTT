#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

const int DHTPIN = 5;
const int DHTTYPE = DHT11;

DHT dht(DHTPIN, DHTTYPE);
StaticJsonDocument<200> json;
WebServer webServer(80);
char* ssid = "P401";
char* pass = "thaonghiadung";
//=========Biến chứa mã HTLM Website==//

const char *broker = "broker.hivemq.com";
WiFiClient tcpclient;
PubSubClient mqttclient(tcpclient);
const char *ID = "Dung-Nguyen-Deviot";
const char *topic[4] = {"ESP32/DungNguyen/DEVIOT/LED_D2", "ESP32/DungNguyen/DEVIOT/LED_D4", "ESP32/DungNguyen/DEVIOT/NHIETDO", "ESP32/DungNguyen/DEVIOT/DOAM"};

void reconnect()
{
  Serial.println("Lost Connection");
  while(!mqttclient.connected())
  {
    Serial.print("Retry Connection");
    if(mqttclient.connect(ID))
    {
      for(int i = 0 ; i < 4 ; i++)
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
void callbackData(char* topic, byte *payLoad, unsigned int sizeData);
const char MainPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <title>HOME PAGE</title>
    <style>
        body {
            text-align: center;
            background: url(https://i.pinimg.com/originals/3b/8a/d2/3b8ad2c7b1be2caf24321c852103598a.jpg);
            background-size: 100%;
            background-repeat: no-repeat;
            padding: 15px;
        }

        h1 {
            color: #003399;
        }

        a {
            text-decoration: none;
            color: #FFFFFF;
        }

        .button {
            background-color: #4CAF50;
            /* Green */
            border: none;
            color: white;
            padding: 16px 40px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 20px;
            margin: 8px 10px;
            transition-duration: 0.4s;
            cursor: pointer;
        }

        .bt_on {
            padding: 16px 47px;
            background-color: rgba(57, 170, 35, 0.575);
            color: black;
            border: 2px solid #4CAF50;
            border-radius: 14px;
        }

        .bt_on:hover {
            background-color: #0e7712;
            color: white;
        }

        .bt_off {
            padding: 16px 40px;
            background-color: rgba(35, 150, 170, 0.575);
            color: black;
            border: 2px solid #045569;
            border-radius: 14px;
        }

        .bt_off:hover {
            background-color: #045569;
            color: white;
        }

        .TEM {
            border-radius: 4px;
            padding: 5px;
            width: 50px;
        }
    </style>
    <meta charset="UTF-8">
</head>

<body>
    <h1 style="color:white;">ESP32 WEBSERVER</h1><br><br>

    <div style="color: rgb(216, 207, 231); font-size: 25px;">LED1 STATUS : <b>
            <pan id="trangthaiD1">
                <pan>
        </b></div><br>
    
    <div>
        <button class="button bt_on" onclick="getdata('onD1')">ON</button>
        <button class="button bt_off" onclick="getdata('offD1')">OFF</button>
    </div><br>

    <div style="color: rgb(216, 207, 231); font-size: 25px;">LED2 STATUS: <b>
            <pan id="trangthaiD2">
                <pan>
        </b></div><br>
    <div>
        <button class="button bt_on" onclick="getdata('onD2')">ON</button>
        <button class="button bt_off" onclick="getdata('offD2')">OFF</button>
    </div>
    <div id="reponsetext"></div><br><br>

    <div>
        <h2 style="font-size: 40px; rgb(54, 57, 216);">
            Temperature :
            <pan id="nhietdo">
                </pan>
                    Degrees Celsius
        </h2>
    </div>

    <div>
        <h2 style="font-size: 40px; rgb(54, 57, 216);">
            Humidity :
            <pan id="doam">
                </pan>
                  %
        </h2>
    </div>
    <script>
        //-----------Tạo đối tượng request----------------
        function create_obj() {
            td = navigator.appName;
            if (td == "Microsoft Internet Explorer") {
                obj = new ActiveXObject("Microsoft.XMLHTTP");
            } else {
                obj = new XMLHttpRequest();
            }
            return obj;
        }
        var xhttp = create_obj();
        //-----------Thiết lập dữ liệu và gửi request-------
        function getdata(url) {
            xhttp.open("GET", "/" + url, true);
            xhttp.onreadystatechange = process;//nhận reponse 
            xhttp.send();
        }
        //-----------Kiểm tra response--------------------
        function process() {
            if (xhttp.readyState == 4 && xhttp.status == 200) {
                //------Updat data sử dụng javascript----------
                ketqua = xhttp.responseText;
                document.getElementById("reponsetext").innerHTML = ketqua;
            }
        }
        //------------Kiểm tra trạng thái chân D1 D2------

        function getstatusD1D2() {
            xhttp.open("GET", "getstatusD1D2", true);
            xhttp.onreadystatechange = process_json;
            xhttp.send();
        }
        //-----------Kiểm tra response-------------------------------
        function process_json() {
            if (xhttp.readyState == 4 && xhttp.status == 200) {
                //------Update data sử dụng javascript-------------------
                var trangthaiD1D2_JSON = xhttp.responseText;
                var Obj = JSON.parse(trangthaiD1D2_JSON);
                document.getElementById("trangthaiD1").innerHTML = Obj.D1;
                document.getElementById("trangthaiD2").innerHTML = Obj.D2;
                document.getElementById("nhietdo").innerHTML = Obj.NHIETDO;
                document.getElementById("doam").innerHTML = Obj.DOAM;
            }
        }
        //---Ham update duu lieu tu dong---
        setInterval(function () {
            getstatusD1D2();
        }, 1000);
    </script>
</body>

</html>
)rawliteral";
//=========================================//
void mainpage();
void on_D1();
void off_D1();
void on_D2();
void off_D2();
void get_statusD1D2();

void setup() {
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(2,HIGH);
  digitalWrite(4,HIGH);
  WiFi.begin(ssid,pass);
  Serial.begin(115200);
  Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print("...");
  }
  Serial.println(WiFi.localIP());

  webServer.on("/",mainpage);
  webServer.on("/onD1",on_D1);
  webServer.on("/offD1",off_D1);
  webServer.on("/onD2",on_D2);
  webServer.on("/offD2",off_D2);
  webServer.on("/getstatusD1D2",get_statusD1D2);
  webServer.begin();
  dht.begin();
  
  mqttclient.setServer(broker, 1883);
  mqttclient.setCallback(callbackData);
  mqttclient.connect(ID);
  for(int i = 0 ; i < 4 ; i++)
  {
    mqttclient.subscribe(topic[i]);
  }
}

String LED1_Status = "ON";
String LED2_Status = "ON";
void loop() {
  webServer.handleClient();
  if(!mqttclient.connected())
  {
    reconnect();
  }
  mqttclient.loop();
}
//==========Chương trình con=================//
void mainpage(){
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}
void on_D1(){
  digitalWrite(2,1);
  LED1_Status = "ON";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D2", "21");
  webServer.send(200,"text/html","CHÂN D1 ĐÃ ON");
}
void off_D1(){
  digitalWrite(2,0);
  LED1_Status = "OFF";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D2", "20");
  webServer.send(200,"text/html","CHÂN D1 ĐÃ OFF");
}
void on_D2(){
  digitalWrite(4,1);
  LED2_Status = "ON";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D4", "41");
  webServer.send(200,"text/html","CHÂN D2 ĐÃ ON");
}
void off_D2(){
  digitalWrite(4,0);
  LED2_Status = "OFF";
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/LED_D4", "40");
  webServer.send(200,"text/html","CHÂN D2 ĐÃ OFF");
}
void get_statusD1D2(){
//  String s = "{\"D1\": \""+ LED1_Status +"\",\"D2\": \""+ LED2_Status +"\",\"TEM\"}";
  json["D1"] = LED1_Status;
  json["D2"] = LED2_Status;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  json["NHIETDO"] = String(t);
  json["DOAM"] = String(h);
  
  String s = "";
  serializeJson(json, s);
  webServer.send(200,"application/json",s);
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/NHIETDO", String(t).c_str());
  mqttclient.publish("ESP32/DungNguyen/DEVIOT/DOAM", String(h).c_str());
}

void callbackData(char* topic, byte *payLoad, unsigned int sizeData)
{
  String s = "";
  for(int i = 0 ; i < sizeData; i++)
  {
    s += (char) payLoad[i];
  }
  Serial.println(topic);
  Serial.println(s);
  Serial.println("----------------------------------");
  
  if(s[0] == '2')
  {
    if(s[1] == '1')
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
  else if(s[0] == '4')
  {
    if(s[1] == '1')
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