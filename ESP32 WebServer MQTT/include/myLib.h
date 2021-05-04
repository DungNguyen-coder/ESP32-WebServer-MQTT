#include <Arduino.h>

void callbackData(char *topic, byte *payLoad, unsigned int sizeData);
void reconnect();

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