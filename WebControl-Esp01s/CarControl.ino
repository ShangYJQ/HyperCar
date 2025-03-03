#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>

#ifndef STASSID

//设置 sta
#define STASSID "YYJJQQ"
#define STAPSK "YYYJJJQQQ"

//设置 ap
#define STASSID_AP "EspCar"
#define STAPSK_AP "12345678"

#endif

const char *ssid = STASSID;
const char *password = STAPSK;


ESP8266WebServer server(80);

StreamString indexHtml = R"rawliteral(

<meta charset='UTF-8'>
<meta content='width=device-width,initial-scale=1' name='viewport'>
<title>遥控小车控制器</title>

<style>
    body {
        font-family: Arial, sans-serif;
        background-color: #b3b3b39d;
        text-align: center;
        margin-top: 50px;
    }

    .control-slider {
        width: 200px;
        margin: 20px;
        appearance: none;
        border: 3px solid #3939398a;
        border-radius: 6px;
    }

    .control-slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 20px;
        height: 20px;
        background: #535353;
        border-radius: 0;
        cursor: pointer;
    }

    .control-slider:focus {
        outline: none;
    }

    .container {
        display: flex;
        flex-direction: row;
        align-items: center;
    }

    .row {
        display: flex;
        justify-content: center;
        width: 100%;
    }

    .label {
        font-size: 20px;
        margin: 10px;
    }
</style>
<div class='container' style="flex-direction: column;">
    <div class='container'>
        <h1>遥控小车控制器</h1>
    </div>
    <div class='container' style="margin-bottom: 120px;">
        <h3>
            左右参数 <span id="lr"></span> 前后参数 <span id="fb"></span>
        </h3>
    </div>
    <div class='container'>
        <div class='row'>
            <label class='label'>左右</label>
            <input type='range' class='control-slider' id='left-right' min='0' max='100' value='0'>
        </div>
        <div class='row'>
            <label class='label'>前后</label>
            <input type='range' class='control-slider' style="transform: rotate(270deg);" id='forward-backward' min='0'
                max='100' value='0'>
        </div>
    </div>
</div>
<script>
    var xmlHttpReq = new XMLHttpRequest();
    var forwardBackwardValue = 0;
    var leftRightValue = 0;
    var last_forwardBackwardValue = 0;
    var last_leftRightValue = 0;
    var isUserInteractingfb = false;
    var isUserInteractinglr = false;
    setInterval(function () {
        if ((forwardBackwardValue == last_forwardBackwardValue) && (!isUserInteractingfb)) {
            document.getElementById('forward-backward').value = 50;
            forwardBackwardValue = 50;
        }
        if ((leftRightValue == last_leftRightValue) && (!isUserInteractinglr)) {
            document.getElementById('left-right').value = 50;
            leftRightValue = 50;
        }

        if (!((leftRightValue == last_leftRightValue) && (forwardBackwardValue == last_forwardBackwardValue))) {
            sendAction(forwardBackwardValue, leftRightValue);
        }
        last_leftRightValue = leftRightValue;
        last_forwardBackwardValue = forwardBackwardValue;
    }, 20);
    function sendAction(forwardBackward, leftRight) {
        xmlHttpReq.open('GET', '/sendAction?action=' + forwardBackward.toString().padStart(3, '0') + leftRight.toString().padStart(3, '0'), true);
        xmlHttpReq.send();
        document.getElementById("lr").innerHTML = leftRightValue;
        document.getElementById("fb").innerHTML = forwardBackwardValue;
    }
    document.getElementById('forward-backward').addEventListener('input', function () { forwardBackwardValue = this.value; });
    document.getElementById('left-right').addEventListener('input', function () { leftRightValue = this.value; });
    document.getElementById('forward-backward').addEventListener('mousedown', function () { isUserInteractingfb = true; });
    document.getElementById('forward-backward').addEventListener('mouseup', function () { isUserInteractingfb = false; });
    document.getElementById('forward-backward').addEventListener('touchstart', function () { isUserInteractingfb = true; });
    document.getElementById('forward-backward').addEventListener('touchend', function () { isUserInteractingfb = false; });
    document.getElementById('left-right').addEventListener('mousedown', function () { isUserInteractinglr = true; });
    document.getElementById('left-right').addEventListener('mouseup', function () { isUserInteractinglr = false; });
    document.getElementById('left-right').addEventListener('touchstart', function () { isUserInteractinglr = true; });
    document.getElementById('left-right').addEventListener('touchend', function () { isUserInteractinglr = false; });
</script>

)rawliteral";

// 处理根页面请求
void handleRoot() {
  server.send(200, "text/html", indexHtml.c_str());
}

//处理action请求
void handleAction() {
  String act = server.arg("action");
  if (act != "")
    Serial.print(act+"\n");
  else Serial.print("e");
}


void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);  // 启用 LED 输出，调试时可用
  digitalWrite(LED_BUILTIN, 1);
  Serial.begin(115200);


  WiFi.softAP(STASSID_AP, STAPSK_AP);  // 设置 ESP8266 为 Access Point 模式 

  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);

  // 等待连接
  // while (WiFi.status() != WL_CONNECTED) {
    // delay(250);
    // Serial.print(".");
  // }

  MDNS.begin("car");  //可通过 http://car.local 来访问本机

  server.on("/", handleRoot);              // 主页请求
  server.on("/sendAction", handleAction);  // 接受按钮点击的 GET 请求

  server.begin();

  Serial.println("");
  Serial.println("R");
  digitalWrite(LED_BUILTIN, 0);
  // Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();  // 处理 HTTP 请求
  MDNS.update();          // 更新 mDNS 服务
}
